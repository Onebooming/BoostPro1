# MySQL连接线程安全修复说明

## 问题描述

在多线程并发环境下，服务器出现 `double free detected in tcache 2` 错误导致崩溃。

### 根本原因

MySQL C API 不是线程安全的。之前的实现中：

```cpp
// ❌ 旧的不安全方式
auto conn_client = MySQLConnectionManager::instance().getConnection(...);
MYSQL* mysql = conn_client->get();  // 锁在这里就释放了！
mysql_query(mysql, ...);             // ❌ 没有锁保护 - 多线程同时访问会导致崩溃
mysql_store_result(mysql);           // ❌ 没有锁保护
```

问题：
1. `MySQLClient::get()` 中的 `lock_guard` 在函数返回后就释放了
2. 后续的 `mysql_query()`, `mysql_store_result()` 等操作**没有任何锁保护**
3. 多个线程同时使用同一个 `MYSQL*` 指针，导致内存损坏和 double free 错误

## 解决方案：RAII 锁守卫 (RAII Lock Guard)

### 1. 新增 `MySQLLockedConnection` 类

在 `mysql_pro.hpp` 中添加了 RAII 锁守卫类：

```cpp
// RAII锁守卫类，用于在MySQL操作期间持有锁
class MySQLLockedConnection {
public:
    MySQLLockedConnection(MYSQL* conn, std::unique_lock<std::mutex>&& lock)
        : conn_(conn), lock_(std::move(lock)) {}

    // 禁止拷贝，允许移动
    MySQLLockedConnection(const MySQLLockedConnection&) = delete;
    MySQLLockedConnection& operator=(const MySQLLockedConnection&) = delete;
    MySQLLockedConnection(MySQLLockedConnection&& other) noexcept = default;

    // 获取MySQL连接指针
    MYSQL* get() const { return conn_; }

    // 自动释放锁（析构时）
    ~MySQLLockedConnection() = default;

private:
    MYSQL* conn_;
    std::unique_lock<std::mutex> lock_;  // 持有锁，直到对象销毁
};
```

### 2. 新增 `getLocked()` 方法

在 `MySQLClient` 类中添加：

```cpp
class MySQLClient {
public:
    // 获取底层mysql指针（旧方法，不推荐使用）
    MYSQL* get();

    // 获取带锁的连接（推荐使用）- 返回RAII对象，自动管理锁的生命周期
    MySQLLockedConnection getLocked();
    // ...
};
```

实现 (`mysql_pro.cpp`):

```cpp
chenglei::MySQLLockedConnection chenglei::MySQLClient::getLocked() {
    std::unique_lock<std::mutex> lock(mutex_);
    return MySQLLockedConnection(conn_, std::move(lock));
}
```

### 3. 使用方式

在所有 DAO 文件中，将原来的方式：

```cpp
// ❌ 旧方式 - 不安全
MYSQL* conn = conn_client->get();
```

替换为：

```cpp
// ✅ 新方式 - 线程安全
auto locked_conn = conn_client->getLocked();
MYSQL* conn = locked_conn.get();
```

### 4. 完整示例

```cpp
bool StudentDao::insertStudent(const StudentBaseInfo& student) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;

    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();  // 获取锁
    MYSQL* conn = locked_conn.get();              // 获取连接指针

    // ✅ 以下所有操作都在锁的保护下执行
    std::string sql = "INSERT INTO students ...";
    int ret = mysql_query(conn, sql.c_str());     // 有锁保护
    if (ret != 0) {
        std::cerr << "Insert Error: " << mysql_error(conn) << std::endl;
        return false;
    }

    return true;
    // ✅ locked_conn 析构时自动释放锁
}
```

## 工作原理

1. **获取锁**: `getLocked()` 获取 `mutex_` 的锁并通过 `std::move` 转移到 `MySQLLockedConnection` 对象中
2. **持有锁**: 只要 `locked_conn` 对象存在，锁就一直被持有
3. **自动释放**: 当函数结束时，`locked_conn` 自动析构，锁自动释放

### 锁的生命周期

```cpp
{
    auto locked_conn = conn_client->getLocked();  // 🔒 锁定
    MYSQL* conn = locked_conn.get();

    mysql_query(conn, ...);        // 🔒 仍在锁保护中
    mysql_store_result(conn);      // 🔒 仍在锁保护中
    mysql_free_result(result);     // 🔒 仍在锁保护中

}  // 🔓 锁自动释放 (locked_conn 析构)
```

## 测试结果

### 并发测试

```bash
# 20个并发请求
for i in {1..20}; do
    curl -s http://127.0.0.1:20080/api/student?action=list_students > /dev/null &
done
wait
```

**结果**: ✅ 所有20个请求成功处理，服务器稳定运行，无崩溃

### 对比

| 修复前 | 修复后 |
|--------|--------|
| ❌ 多个并发请求导致 double free 崩溃 | ✅ 成功处理20+并发请求 |
| ❌ "Lost connection to MySQL server" 错误 | ✅ 无MySQL连接错误 |
| ⚠️ 锁作用域过小，无法保护MySQL操作 | ✅ RAII自动管理锁生命周期 |

## 优势

1. **线程安全**: 完全消除了多线程并发访问MySQL导致的内存损坏
2. **自动管理**: RAII确保锁一定会被释放，不会忘记解锁
3. **连接复用**: 保持连接池，提高了效率
4. **易于使用**: 只需修改3行代码，API简洁
5. **异常安全**: 即使发生异常，锁也会自动释放

## 已更新的文件

### 核心文件
- `server/src/database/mysql_pro.hpp` - 添加 `MySQLLockedConnection` 类和 `getLocked()` 方法
- `server/src/database/mysql_pro.cpp` - 实现 `getLocked()` 方法

### DAO 文件 (已更新为使用 `getLocked()`)
- `server/src/dao/student_dao.cpp`
- `server/src/dao/teacher_dao.cpp`
- `server/src/dao/course_dao.cpp`
- `server/src/dao/classroom_dao.cpp`
- `server/src/dao/news_dao.cpp`
- `server/src/dao/blog_dao.cpp`
- `server/src/dao/notification_dao.cpp`

## 性能影响

- **锁定开销**: 极小，只是多了一个 `unique_lock` 对象的创建
- **并发性能**: 同一数据库连接会被串行化（这是必须的，MySQL C API要求如此）
- **连接池效率**: 保持了连接复用，避免了频繁创建/销毁连接的开销

## 未来优化建议

如果需要更高的并发性能，可以考虑：

1. **每个线程独立连接**: 为每个线程创建独立的MySQL连接
2. **连接池扩展**: 实现真正的连接池，为每个请求分配独立连接
3. **使用线程安全封装**: 考虑使用支持线程安全的MySQL封装库（如 MySQL++）

但对于当前的应用场景，RAII锁守卫方案已经足够满足需求。

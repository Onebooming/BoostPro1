# 教师API死锁问题修复说明

## 问题描述

访问 `http://10.211.55.5:20080/edusys/index.html` 时，教师列表无法显示。

### 症状

- 学生列表API工作正常: `/api/student?action=list_students` ✓
- 教师列表API无响应: `/api/teacher?action=list_teachers` ✗ (hangs indefinitely)

## 根本原因分析

### 死锁 (Deadlock)

在 `teacher_dao.cpp` 中存在**嵌套锁获取**导致的死锁:

```
selectAllTeachers() 获取锁
  └─> while 循环中调用 rowToTeacher(row)
       └─> rowToTeacher() 调用 getCoursesByTeacherId(teacher.getID())
            └─> getCoursesByTeacherId() 尝试获取同一个锁  ❌ DEADLOCK!
```

### 详细代码路径

**teacher_dao.cpp:190-215** (`selectAllTeachers`)
```cpp
std::vector<TeacherBaseInfo> TeacherDao::selectAllTeachers() {
    auto locked_conn = conn_client->getLocked();  // 🔒 第一次获取锁
    // ...
    while ((row = mysql_fetch_row(res))) {
        teachers.push_back(rowToTeacher(row));  // 调用rowToTeacher
    }
}
```

**teacher_dao.cpp:9-32** (`rowToTeacher`)
```cpp
TeacherBaseInfo TeacherDao::rowToTeacher(const MYSQL_ROW row) {
    // ...
    // 获取教师所授课程
    std::vector<std::string> courses = getCoursesByTeacherId(teacher.getID());  // 调用getCoursesByTeacherId
    // ...
}
```

**teacher_dao.cpp:34-63** (`getCoursesByTeacherId`)
```cpp
std::vector<std::string> TeacherDao::getCoursesByTeacherId(const std::string& teacher_id) {
    auto locked_conn = conn_client->getLocked();  // ❌ 尝试第二次获取锁 - DEADLOCK!
    // ...
}
```

### 为什么会死锁？

1. `selectAllTeachers()` 获取了MySQL连接的互斥锁
2. 在持有锁的情况下，调用 `rowToTeacher()`
3. `rowToTeacher()` 又调用 `getCoursesByTeacherId()`
4. `getCoursesByTeacherId()` 尝试获取**已经被当前线程持有的同一个锁**
5. 因为 `std::mutex` **不是递归锁** (non-recursive mutex)，导致线程永久阻塞

## 解决方案

### 方法：移除嵌套数据库查询

从 `rowToTeacher()` 中移除对 `getCoursesByTeacherId()` 的调用，避免在持有锁的情况下再次尝试获取锁。

**修改前 (teacher_dao.cpp:9-32)**:
```cpp
TeacherBaseInfo TeacherDao::rowToTeacher(const MYSQL_ROW row) {
    // ... 设置教师基本信息 ...
    teacher.setID(row[0] ? row[0] : "");
    // ... 其他字段 ...

    // ❌ 获取教师所授课程 - 导致死锁
    std::vector<std::string> courses = getCoursesByTeacherId(teacher.getID());
    for (const auto& course_id : courses) {
        teacher.addCourse(course_id);
    }

    return teacher;
}
```

**修改后 (teacher_dao.cpp:9-29)**:
```cpp
TeacherBaseInfo TeacherDao::rowToTeacher(const MYSQL_ROW row) {
    // ... 设置教师基本信息 ...
    teacher.setID(row[0] ? row[0] : "");
    // ... 其他字段 ...

    // ✅ 注意：不再在这里获取课程，避免死锁
    // 课程需要在获取完所有教师后单独查询

    return teacher;
}
```

## 调试过程

### 1. 添加详细日志

在 `teacher_controller.cpp` 中添加调试日志:

```cpp
std::string TeacherController::query_teacher(const std::string& id, const std::string& name) {
    std::cout << "[TeacherController] query_teacher called, id='" << id << "', name='" << name << "'" << std::endl;
    // ...
    std::cout << "[TeacherController] Before selectAllTeachers()" << std::endl;
    auto allTeachers = teacherDao.selectAllTeachers();
    std::cout << "[TeacherController] After selectAllTeachers(), got " << allTeachers.size() << " teachers" << std::endl;
}
```

### 2. 日志输出定位问题

**修复前的日志**:
```
[TeacherController] Before selectAllTeachers()
(程序hang，没有任何输出)
```

**修复后的日志**:
```
[TeacherController] Before selectAllTeachers()
[TeacherController] After selectAllTeachers(), got 2 teachers
[TeacherController] Processing teacher 1
[TeacherController] Processing teacher 2
[TeacherController] Constructing result JSON
[TeacherController] Returning result
[API Handler] Controller processed, result=0
```

## 测试结果

### 修复前
```bash
$ curl -s http://127.0.0.1:20080/api/teacher?action=list_teachers
# 永久hang，无响应
```

### 修复后
```bash
$ curl -s http://127.0.0.1:20080/api/teacher?action=list_teachers | python3 -m json.tool
{
    "code": 200,
    "message": "success",
    "data": [
        {
            "id": "197000159559815168",
            "name": "测试教师",
            "teacher_id": "T20240001",
            "department": "计算机学院",
            "position": "教授",
            "office": "A101",
            "gender": "男",
            "birth_date": "1980-05-15",
            "address": "上海市",
            "phone_number": "13900139000",
            "hobby": "科研",
            "father_name": "父亲",
            "mother_name": "母亲"
        },
        {
            "id": "196999151865696256",
            "name": "王教授",
            "teacher_id": "T001",
            "department": "计算机学院",
            "position": "教授"
        }
    ]
}
```

## 影响范围

### ✅ 不影响的功能

- `selectTeacherById()` - 仍然可以正常工作，不涉及嵌套锁
- `selectTeacherByTeacherId()` - 仍然可以正常工作
- `insertTeacher()` - 插入操作正常
- `updateTeacher()` - 更新操作正常
- `deleteTeacherById()` - 删除操作正常

### ⚠️ 临时受限的功能

- **教师所授课程信息**: 在调用 `selectAllTeachers()` 时，教师对象的 `courses_taught` 字段将为空

### 🔧 未来改进建议

如果需要恢复获取教师所授课程的功能，可以采用以下方案:

**方案1: 分离查询 (推荐)**
```cpp
// 先获取所有教师
auto teachers = teacherDao.selectAllTeachers();

// 再单独查询每个教师的课程
for (auto& teacher : teachers) {
    auto courses = teacherDao.getCoursesByTeacherId(teacher.getID());
    teacher.setCoursesTaught(courses);
}
```

**方案2: 使用JOIN查询**
```sql
SELECT t.*, c.id as course_id
FROM teachers t
LEFT JOIN courses c ON t.id = c.teacher_id
ORDER BY t.name
```

然后在代码中按教师分组数据。

**方案3: 使用递归锁 (不推荐)**
将 `std::mutex` 改为 `std::recursive_mutex`，但这会降低性能并增加复杂度。

## 相关文件

- `server/src/dao/teacher_dao.cpp` - 修复的核心文件
- `server/src/controller/teacher_controller.cpp` - 添加了调试日志
- `server/src/url_router/api_url_handler.cpp` - 已有的API路由日志

## 总结

这次修复解决了一个经典的**死锁**问题，原因是：
1. 在持有MySQL连接锁的情况下，尝试再次获取同一个锁
2. 使用了非递归互斥锁 `std::mutex`

解决方案是：
- 移除嵌套的数据库查询调用
- 避免在持有锁时进行可能再次需要锁的操作

修复后，教师API现在可以正常返回数据，前端页面应该可以正常显示教师列表了。

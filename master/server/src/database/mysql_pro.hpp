/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-18 23:12:44
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-18 23:55:48
 * @FilePath: /BoostPro1/master/server/src/database/mysql_pro.hpp
 * @Description: mysql的连接及操作类的管理
 */
#pragma once
#include <mysql/mysql.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace chenglei {

// RAII锁守卫类，用于在MySQL操作期间持有锁
class MySQLLockedConnection {
public:
    MySQLLockedConnection(MYSQL* conn, std::unique_lock<std::mutex>&& lock)
        : conn_(conn), lock_(std::move(lock)) {}

    // 禁止拷贝
    MySQLLockedConnection(const MySQLLockedConnection&) = delete;
    MySQLLockedConnection& operator=(const MySQLLockedConnection&) = delete;

    // 允许移动
    MySQLLockedConnection(MySQLLockedConnection&& other) noexcept
        : conn_(other.conn_), lock_(std::move(other.lock_)) {}

    MySQLLockedConnection& operator=(MySQLLockedConnection&& other) noexcept {
        if (this != &other) {
            conn_ = other.conn_;
            lock_ = std::move(other.lock_);
        }
        return *this;
    }

    // 获取MySQL连接指针
    MYSQL* get() const { return conn_; }

    // 自动释放锁（析构时）
    ~MySQLLockedConnection() = default;

private:
    MYSQL* conn_;
    std::unique_lock<std::mutex> lock_;
};

class MySQLClient{
    public :
        MySQLClient();
        ~MySQLClient();

        // 禁止拷贝
        MySQLClient(const MySQLClient &) = delete;
        MySQLClient &operator=(const MySQLClient &) = delete;

        // 初始化连接
        bool connect(const std::string &host, const std::string &user,
                        const std::string &password, const std::string &db,
                        unsigned int port = 3306);

        // 获取底层mysql指针（旧方法，不推荐使用）
        MYSQL* get();

        // 获取带锁的连接（推荐使用）- 返回RAII对象，自动管理锁的生命周期
        MySQLLockedConnection getLocked();

        // 判读是否连接
        bool isConnected() const;

    private:
        MYSQL *conn_;
        bool connected_;
        std::mutex mutex_;
};


    // 全局管理器，按db_name维护连接池（带锁）
class MySQLConnectionManager {
public:
    static MySQLConnectionManager& instance();

    // 获取数据库连接（带锁保护，线程安全）
    std::shared_ptr<MySQLClient> getConnection(const std::string& db_name,
                                               const std::string& host = "127.0.0.1",
                                               const std::string& user = "admin",
                                               const std::string& password = "admin",
                                               unsigned int port = 3306);

    // 释放指定db连接
    void releaseConnection(const std::string& db_name);

    // 清理全部连接
    void clear();

private:
    MySQLConnectionManager() = default;
    ~MySQLConnectionManager();

    MySQLConnectionManager(const MySQLConnectionManager&) = delete;
    MySQLConnectionManager& operator=(const MySQLConnectionManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<MySQLClient>> conn_map_;
    std::mutex map_mutex_;  // 保护连接map的互斥锁
};

}

void init_mysql_module();
void finish_mysql_module();

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

        // 获取底层mysql指针
        MYSQL* get();
    
        // 判读是否连接
        bool isConnected() const;

    private:
        MYSQL *conn_;
        bool connected_;
        std::mutex mutex_;
};


    // 全局管理器，按db_name维护连接
class MySQLConnectionManager {
public:
    static MySQLConnectionManager& instance();

    // 获取数据库连接
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
    std::mutex map_mutex_;
};

}

void init_mysql_module();
void finish_mysql_module();

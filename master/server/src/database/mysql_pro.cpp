/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-18 23:25:02
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-18 23:55:38
 * @FilePath: /BoostPro1/master/server/src/database/mysql_pro.cpp
 * @Description: mysql连接处理
 */
#include "mysql_pro.hpp"
#include <iostream>

namespace chenglei {

////////////////////// MySQLClient //////////////////////

chenglei::MySQLClient::MySQLClient() : conn_(nullptr), connected_(false) {}

chenglei::MySQLClient::~MySQLClient() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (conn_) {
        mysql_close(conn_);
        conn_ = nullptr;
    }
    connected_ = false;
}

bool chenglei::MySQLClient::connect(const std::string& host, const std::string& user,
                          const std::string& password, const std::string& db,
                          unsigned int port) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (conn_) {
        mysql_close(conn_);
        conn_ = nullptr;
    }
    conn_ = mysql_init(nullptr);
    if (!conn_) {
        std::cerr << "MySQL init failed\n";
        connected_ = false;
        return false;
    }
    if (!mysql_real_connect(conn_, host.c_str(), user.c_str(), password.c_str(),
                            db.c_str(), port, nullptr, 0)) {
        std::cerr << "MySQL connect failed: " << mysql_error(conn_) << "\n";
        mysql_close(conn_);
        conn_ = nullptr;
        connected_ = false;
        return false;
    }
    connected_ = true;
    return true;
}

MYSQL* chenglei::MySQLClient::get() {
    std::lock_guard<std::mutex> lock(mutex_);
    return conn_;
}

bool chenglei::MySQLClient::isConnected() const {
    return connected_;
}

////////////////////// MySQLConnectionManager //////////////////////

chenglei::MySQLConnectionManager::~MySQLConnectionManager() {
    clear();
}

chenglei::MySQLConnectionManager& chenglei::MySQLConnectionManager::instance() {
    static MySQLConnectionManager inst;
    return inst;
}

std::shared_ptr<MySQLClient> chenglei::MySQLConnectionManager::getConnection(const std::string& db_name,
                                                                  const std::string& host,
                                                                  const std::string& user,
                                                                  const std::string& password,
                                                                  unsigned int port) {
    std::lock_guard<std::mutex> lock(map_mutex_);
    auto it = conn_map_.find(db_name);
    if (it != conn_map_.end() && it->second && it->second->isConnected()) {
        return it->second;
    }
    // 新建连接
    auto client = std::make_shared<MySQLClient>();
    if (!client->connect(host, user, password, db_name, port)) {
        std::cerr << "Failed to create MySQL connection for db: " << db_name << std::endl;
        return nullptr;
    }
    conn_map_[db_name] = client;
    return client;
}

void chenglei::MySQLConnectionManager::releaseConnection(const std::string& db_name) {
    std::lock_guard<std::mutex> lock(map_mutex_);
    auto it = conn_map_.find(db_name);
    if (it != conn_map_.end()) {
        conn_map_.erase(it);
    }
}

void chenglei::MySQLConnectionManager::clear() {
    std::lock_guard<std::mutex> lock(map_mutex_);
    conn_map_.clear();
}

}

void init_mysql_module() {
    chenglei::MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
}

void finish_mysql_module() {
    chenglei::MySQLConnectionManager::instance().releaseConnection("boostpro1");
}
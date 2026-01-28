/*
 * @FileName: notification_dao.cpp
 * @Description: 通知数据访问对象实现
 */
#include "notification_dao.hpp"
#include "database/mysql_pro.hpp"
#include <iostream>

namespace chenglei {

/**
 * 创建通知表
 */
bool NotificationDao::createTable() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) {
        std::cerr << "Failed to get MySQL connection" << std::endl;
        return false;
    }
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql =
        "CREATE TABLE IF NOT EXISTS notifications ("
        "id VARCHAR(64) PRIMARY KEY COMMENT '通知ID',"
        "title VARCHAR(200) NOT NULL COMMENT '通知标题',"
        "content TEXT COMMENT '通知内容',"
        "type VARCHAR(20) NOT NULL DEFAULT 'notice' COMMENT '通知类型:notice-通知,news-新闻,emergency-紧急通知',"
        "publisher VARCHAR(100) COMMENT '发布人',"
        "publisher_id VARCHAR(64) COMMENT '发布人ID',"
        "priority INT DEFAULT 2 COMMENT '优先级:1-低,2-中,3-高',"
        "is_top BOOLEAN DEFAULT FALSE COMMENT '是否置顶',"
        "is_published BOOLEAN DEFAULT FALSE COMMENT '是否已发布',"
        "view_count INT DEFAULT 0 COMMENT '浏览次数',"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',"
        "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',"
        "published_at DATETIME COMMENT '发布时间'"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='通知公告表'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Create notifications table failed: " << mysql_error(mysql) << std::endl;
        return false;
    }
    return true;
}

/**
 * 插入通知
 */
bool NotificationDao::insertNotification(const Notification& notification) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string escapedTitle = escapeSQL(mysql, notification.title);
    std::string escapedContent = escapeSQL(mysql, notification.content);
    std::string escapedPublisher = escapeSQL(mysql, notification.publisher);
    std::string escapedPublisherId = escapeSQL(mysql, notification.publisher_id);

    std::string sql = "INSERT INTO notifications (id, title, content, type, publisher, publisher_id, "
                     "priority, is_top, is_published, view_count, published_at) VALUES ('" +
                     notification.id + "', '" + escapedTitle + "', '" + escapedContent + "', '" +
                     notification.type + "', '" + escapedPublisher + "', '" +
                     escapedPublisherId + "', " + std::to_string(notification.priority) +
                     ", " + std::to_string(notification.is_top) + ", " +
                     std::to_string(notification.is_published) + ", " +
                     std::to_string(notification.view_count) + ", " +
                     (notification.published_at.empty() ? "NULL" : "'" + notification.published_at + "'") + ")";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Insert notification failed: " << mysql_error(mysql) << std::endl;
        return false;
    }
    return true;
}

/**
 * 更新通知
 */
bool NotificationDao::updateNotification(const Notification& notification) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string escapedTitle = escapeSQL(mysql, notification.title);
    std::string escapedContent = escapeSQL(mysql, notification.content);
    std::string escapedPublisher = escapeSQL(mysql, notification.publisher);
    std::string escapedPublisherId = escapeSQL(mysql, notification.publisher_id);

    std::string sql = "UPDATE notifications SET title='" + escapedTitle +
                     "', content='" + escapedContent + "', type='" + notification.type +
                     "', publisher='" + escapedPublisher + "', publisher_id='" +
                     escapedPublisherId + "', priority=" + std::to_string(notification.priority) +
                     ", is_top=" + std::to_string(notification.is_top) +
                     ", is_published=" + std::to_string(notification.is_published) +
                     ", view_count=" + std::to_string(notification.view_count) +
                     ", published_at=" + (notification.published_at.empty() ? "NULL" : "'" + notification.published_at + "'") +
                     " WHERE id='" + notification.id + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Update notification failed: " << mysql_error(mysql) << std::endl;
        return false;
    }
    return true;
}

/**
 * 删除通知
 */
bool NotificationDao::deleteNotification(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "DELETE FROM notifications WHERE id='" + id + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Delete notification failed: " << mysql_error(mysql) << std::endl;
        return false;
    }
    return true;
}

/**
 * 根据ID查询通知
 */
std::unique_ptr<Notification> NotificationDao::selectNotificationById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return nullptr;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM notifications WHERE id='" + id + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Query notification by id failed: " << mysql_error(mysql) << std::endl;
        return nullptr;
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return nullptr;

    MYSQL_ROW row = mysql_fetch_row(result);
    auto notification = rowToNotification(row);
    mysql_free_result(result);

    return notification;
}

/**
 * 查询所有通知
 */
std::vector<Notification> NotificationDao::selectAllNotifications() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM notifications ORDER BY is_top DESC, priority DESC, created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Query all notifications failed: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Notification> notifications;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        auto notification = rowToNotification(row);
        if (notification) {
            notifications.push_back(*notification);
        }
    }

    mysql_free_result(result);
    return notifications;
}

/**
 * 根据类型查询通知
 */
std::vector<Notification> NotificationDao::selectNotificationsByType(const std::string& type) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM notifications WHERE type='" + type +
                     "' ORDER BY is_top DESC, priority DESC, created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Query notifications by type failed: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Notification> notifications;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        auto notification = rowToNotification(row);
        if (notification) {
            notifications.push_back(*notification);
        }
    }

    mysql_free_result(result);
    return notifications;
}

/**
 * 查询已发布的通知
 */
std::vector<Notification> NotificationDao::selectPublishedNotifications() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM notifications WHERE is_published=1 "
                     "ORDER BY is_top DESC, priority DESC, published_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Query published notifications failed: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Notification> notifications;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        auto notification = rowToNotification(row);
        if (notification) {
            notifications.push_back(*notification);
        }
    }

    mysql_free_result(result);
    return notifications;
}

/**
 * 将数据库行转换为Notification对象
 */
std::unique_ptr<Notification> NotificationDao::rowToNotification(MYSQL_ROW row) {
    if (!row) return nullptr;

    auto notification = std::make_unique<Notification>();
    notification->id = row[0] ? row[0] : "";
    notification->title = row[1] ? row[1] : "";
    notification->content = row[2] ? row[2] : "";
    notification->type = row[3] ? row[3] : "notice";
    notification->publisher = row[4] ? row[4] : "";
    notification->publisher_id = row[5] ? row[5] : "";
    notification->priority = row[6] ? std::atoi(row[6]) : 2;
    notification->is_top = row[7] ? (std::string(row[7]) == "1") : false;
    notification->is_published = row[8] ? (std::string(row[8]) == "1") : false;
    notification->view_count = row[9] ? std::atoi(row[9]) : 0;
    notification->created_at = row[10] ? row[10] : "";
    notification->updated_at = row[11] ? row[11] : "";
    notification->published_at = row[12] ? row[12] : "";

    return notification;
}

/**
 * 转义SQL字符串
 */
std::string NotificationDao::escapeSQL(MYSQL* mysql, const std::string& str) {
    char* escaped = new char[str.length() * 2 + 1];
    mysql_real_escape_string(mysql, escaped, str.c_str(), str.length());
    std::string result(escaped);
    delete[] escaped;
    return result;
}

} // namespace chenglei

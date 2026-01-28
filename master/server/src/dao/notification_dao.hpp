/*
 * @FileName: notification_dao.hpp
 * @Description: 通知数据访问对象
 */
#ifndef NOTIFICATION_DAO_HPP
#define NOTIFICATION_DAO_HPP

#include "entity/notification.hpp"
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <memory>

namespace chenglei {

/**
 * 通知DAO类
 */
class NotificationDao {
private:
    NotificationDao() = default;
    ~NotificationDao() = default;
    NotificationDao(const NotificationDao&) = delete;
    NotificationDao& operator=(const NotificationDao&) = delete;

public:
    /**
     * 获取单例实例
     */
    static NotificationDao& getInstance() {
        static NotificationDao instance;
        return instance;
    }

    /**
     * 创建通知表
     */
    bool createTable();

    /**
     * 插入通知
     */
    bool insertNotification(const Notification& notification);

    /**
     * 更新通知
     */
    bool updateNotification(const Notification& notification);

    /**
     * 删除通知
     */
    bool deleteNotification(const std::string& id);

    /**
     * 根据ID查询通知
     */
    std::unique_ptr<Notification> selectNotificationById(const std::string& id);

    /**
     * 查询所有通知
     */
    std::vector<Notification> selectAllNotifications();

    /**
     * 根据类型查询通知
     */
    std::vector<Notification> selectNotificationsByType(const std::string& type);

    /**
     * 查询已发布的通知
     */
    std::vector<Notification> selectPublishedNotifications();

private:
    /**
     * 将数据库行转换为Notification对象
     */
    std::unique_ptr<Notification> rowToNotification(MYSQL_ROW row);

    /**
     * 转义SQL字符串
     */
    std::string escapeSQL(MYSQL* mysql, const std::string& str);
};

} // namespace chenglei

#endif // NOTIFICATION_DAO_HPP

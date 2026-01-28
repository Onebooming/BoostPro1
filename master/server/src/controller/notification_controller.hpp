/*
 * @FileName: notification_controller.hpp
 * @Description: 通知控制器
 */
#ifndef NOTIFICATION_CONTROLLER_HPP
#define NOTIFICATION_CONTROLLER_HPP

#include "dao/notification_dao.hpp"
#include "utils/snowflake.hpp"
#include "../../../public/json.hpp"
#include <boost/beast/http.hpp>
#include <string>

namespace http = boost::beast::http;
using json = nlohmann::json;

namespace chenglei {

class NotificationController {
private:
    NotificationDao& notificationDao = NotificationDao::getInstance();

public:
    NotificationController() {
        // 不在构造函数中创建表，避免初始化问题
    }

    /**
     * 处理通知相关请求
     */
    http::response<http::string_body> handleRequest(
        const http::request<http::string_body>& req,
        const std::string& action,
        const std::string& requestBody) {

        // 获取所有通知
        if (action == "list_notifications") {
            return listNotifications();
        }

        // 获取已发布通知
        if (action == "list_published_notifications") {
            return listPublishedNotifications();
        }

        // 根据类型获取通知
        if (action == "list_notifications_by_type") {
            return listNotificationsByType(req, requestBody);
        }

        // 根据ID获取通知
        if (action == "query_notification_by_id") {
            return queryNotificationById(req, requestBody);
        }

        // 添加通知
        if (action == "add_notification") {
            return addNotification(requestBody);
        }

        // 更新通知
        if (action == "update_notification") {
            return updateNotification(requestBody);
        }

        // 删除通知
        if (action == "delete_notification") {
            return deleteNotification(requestBody);
        }

        // 发布通知
        if (action == "publish_notification") {
            return publishNotification(requestBody);
        }

        // 增加浏览次数
        if (action == "increment_view_count") {
            return incrementViewCount(requestBody);
        }

        // 未知的action
        return ErrorResponse(400, "Unknown action: " + action);
    }

private:
    /**
     * 获取所有通知
     */
    http::response<http::string_body> listNotifications() {
        // 确保表存在（懒加载）
        static bool tableCreated = false;
        if (!tableCreated) {
            notificationDao.createTable();
            tableCreated = true;
        }

        auto notifications = notificationDao.selectAllNotifications();

        json jsonArray = json::array();
        for (const auto& notification : notifications) {
            jsonArray.push_back(notificationToJson(notification));
        }

        json response;
        response["code"] = 200;
        response["message"] = "success";
        response["data"] = jsonArray;

        return buildJsonResponse(response, 200);
    }

    /**
     * 获取已发布通知
     */
    http::response<http::string_body> listPublishedNotifications() {
        auto notifications = notificationDao.selectPublishedNotifications();

        json jsonArray = json::array();
        for (const auto& notification : notifications) {
            jsonArray.push_back(notificationToJson(notification));
        }

        json response;
        response["code"] = 200;
        response["message"] = "success";
        response["data"] = jsonArray;

        return buildJsonResponse(response, 200);
    }

    /**
     * 根据类型获取通知
     */
    http::response<http::string_body> listNotificationsByType(
        const http::request<http::string_body>& req,
        const std::string& requestBody) {

        try {
            // 从URL参数获取类型
            std::string url = req.target().to_string();
            size_t pos = url.find("type=");
            if (pos == std::string::npos) {
                return ErrorResponse(400, "Missing type parameter");
            }

            std::string type = url.substr(pos + 5);
            // 去除可能的额外参数
            size_t ampPos = type.find('&');
            if (ampPos != std::string::npos) {
                type = type.substr(0, ampPos);
            }

            auto notifications = notificationDao.selectNotificationsByType(type);

            json jsonArray = json::array();
            for (const auto& notification : notifications) {
                jsonArray.push_back(notificationToJson(notification));
            }

            json response;
            response["code"] = 200;
            response["message"] = "success";
            response["data"] = jsonArray;

            return buildJsonResponse(response, 200);
        } catch (const std::exception& e) {
            return ErrorResponse(500, std::string("Error: ") + e.what());
        }
    }

    /**
     * 根据ID查询通知
     */
    http::response<http::string_body> queryNotificationById(
        const http::request<http::string_body>& req,
        const std::string& requestBody) {

        try {
            json data = json::parse(requestBody);
            if (!data.contains("id")) {
                return ErrorResponse(400, "Missing id field");
            }

            std::string id = data["id"];
            auto notification = notificationDao.selectNotificationById(id);

            if (!notification) {
                return ErrorResponse(404, "Notification not found");
            }

            json response;
            response["code"] = 200;
            response["message"] = "success";
            response["data"] = notificationToJson(*notification);

            return buildJsonResponse(response, 200);
        } catch (const std::exception& e) {
            return ErrorResponse(500, std::string("Error: ") + e.what());
        }
    }

    /**
     * 添加通知
     */
    http::response<http::string_body> addNotification(const std::string& requestBody) {
        try {
            json data = json::parse(requestBody);

            // 验证必填字段
            if (!data.contains("title") || !data.contains("content")) {
                return ErrorResponse(400, "Missing required fields: title and content");
            }

            Notification notification;
            notification.id = SnowflakeIDGenerator::instance().next_id();
            notification.title = data["title"];
            notification.content = data["content"];
            notification.type = data.value("type", "notice");
            notification.publisher = data.value("publisher", "系统管理员");
            notification.publisher_id = data.value("publisher_id", "admin");
            notification.priority = data.value("priority", 2);
            notification.is_top = data.value("is_top", false);

            // 如果指定了立即发布
            if (data.value("publish", false)) {
                notification.publish();
            }

            if (notificationDao.insertNotification(notification)) {
                json response;
                response["code"] = 200;
                response["message"] = "Notification added successfully";
                response["data"] = notificationToJson(notification);
                return buildJsonResponse(response, 200);
            } else {
                return ErrorResponse(500, "Failed to add notification");
            }
        } catch (const std::exception& e) {
            return ErrorResponse(500, std::string("Error: ") + e.what());
        }
    }

    /**
     * 更新通知
     */
    http::response<http::string_body> updateNotification(const std::string& requestBody) {
        try {
            json data = json::parse(requestBody);

            if (!data.contains("id")) {
                return ErrorResponse(400, "Missing id field");
            }

            // 先获取现有通知
            auto existingNotification = notificationDao.selectNotificationById(data["id"]);
            if (!existingNotification) {
                return ErrorResponse(404, "Notification not found");
            }

            // 更新字段
            if (data.contains("title")) existingNotification->title = data["title"];
            if (data.contains("content")) existingNotification->content = data["content"];
            if (data.contains("type")) existingNotification->type = data["type"];
            if (data.contains("priority")) existingNotification->priority = data["priority"];
            if (data.contains("is_top")) existingNotification->is_top = data["is_top"];

            if (notificationDao.updateNotification(*existingNotification)) {
                json response;
                response["code"] = 200;
                response["message"] = "Notification updated successfully";
                response["data"] = notificationToJson(*existingNotification);
                return buildJsonResponse(response, 200);
            } else {
                return ErrorResponse(500, "Failed to update notification");
            }
        } catch (const std::exception& e) {
            return ErrorResponse(500, std::string("Error: ") + e.what());
        }
    }

    /**
     * 删除通知
     */
    http::response<http::string_body> deleteNotification(const std::string& requestBody) {
        try {
            json data = json::parse(requestBody);

            if (!data.contains("id")) {
                return ErrorResponse(400, "Missing id field");
            }

            if (notificationDao.deleteNotification(data["id"])) {
                json response;
                response["code"] = 200;
                response["message"] = "Notification deleted successfully";
                return buildJsonResponse(response, 200);
            } else {
                return ErrorResponse(500, "Failed to delete notification");
            }
        } catch (const std::exception& e) {
            return ErrorResponse(500, std::string("Error: ") + e.what());
        }
    }

    /**
     * 发布通知
     */
    http::response<http::string_body> publishNotification(const std::string& requestBody) {
        try {
            json data = json::parse(requestBody);

            if (!data.contains("id")) {
                return ErrorResponse(400, "Missing id field");
            }

            auto notification = notificationDao.selectNotificationById(data["id"]);
            if (!notification) {
                return ErrorResponse(404, "Notification not found");
            }

            notification->publish();

            if (notificationDao.updateNotification(*notification)) {
                json response;
                response["code"] = 200;
                response["message"] = "Notification published successfully";
                response["data"] = notificationToJson(*notification);
                return buildJsonResponse(response, 200);
            } else {
                return ErrorResponse(500, "Failed to publish notification");
            }
        } catch (const std::exception& e) {
            return ErrorResponse(500, std::string("Error: ") + e.what());
        }
    }

    /**
     * 增加浏览次数
     */
    http::response<http::string_body> incrementViewCount(const std::string& requestBody) {
        try {
            json data = json::parse(requestBody);

            if (!data.contains("id")) {
                return ErrorResponse(400, "Missing id field");
            }

            auto notification = notificationDao.selectNotificationById(data["id"]);
            if (!notification) {
                return ErrorResponse(404, "Notification not found");
            }

            notification->incrementViewCount();

            if (notificationDao.updateNotification(*notification)) {
                json response;
                response["code"] = 200;
                response["message"] = "View count incremented";
                response["data"]["view_count"] = notification->view_count;
                return buildJsonResponse(response, 200);
            } else {
                return ErrorResponse(500, "Failed to increment view count");
            }
        } catch (const std::exception& e) {
            return ErrorResponse(500, std::string("Error: ") + e.what());
        }
    }

    /**
     * 将Notification转换为JSON
     */
    json notificationToJson(const Notification& notification) {
        json j;
        j["id"] = notification.id;
        j["title"] = notification.title;
        j["content"] = notification.content;
        j["type"] = notification.type;
        j["publisher"] = notification.publisher;
        j["publisher_id"] = notification.publisher_id;
        j["priority"] = notification.priority;
        j["is_top"] = notification.is_top;
        j["is_published"] = notification.is_published;
        j["view_count"] = notification.view_count;
        j["created_at"] = notification.created_at;
        j["updated_at"] = notification.updated_at;
        j["published_at"] = notification.published_at;
        return j;
    }

    /**
     * 构建JSON响应
     */
    http::response<http::string_body> buildJsonResponse(const json& j, int status) {
        http::response<http::string_body> res;
        res.result(status);
        res.set(http::field::server, "Boost.Beast");
        res.set(http::field::content_type, "application/json");
        res.body() = j.dump();
        res.content_length(res.body().size());
        return res;
    }

    /**
     * 构建错误响应
     */
    http::response<http::string_body> ErrorResponse(int status, const std::string& message) {
        json j;
        j["code"] = status;
        j["error"] = message;
        return buildJsonResponse(j, status);
    }
};

} // namespace chenglei

#endif // NOTIFICATION_CONTROLLER_HPP

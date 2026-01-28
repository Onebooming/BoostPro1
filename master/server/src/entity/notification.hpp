/*
 * @FileName: notification.hpp
 * @Description: 通知公告实体类
 */
#ifndef NOTIFICATION_HPP
#define NOTIFICATION_HPP

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace chenglei {

/**
 * 通知公告实体类
 */
class Notification {
public:
    std::string id;                  // 通知ID
    std::string title;               // 通知标题
    std::string content;             // 通知内容
    std::string type;                // 通知类型：notice-通知, news-新闻, emergency-紧急通知
    std::string publisher;           // 发布人
    std::string publisher_id;        // 发布人ID
    int priority;                    // 优先级：1-低, 2-中, 3-高
    bool is_top;                     // 是否置顶
    bool is_published;               // 是否已发布
    int view_count;                  // 浏览次数
    std::string created_at;          // 创建时间
    std::string updated_at;          // 更新时间
    std::string published_at;        // 发布时间

    Notification() : priority(2), is_top(false), is_published(false), view_count(0) {}

    /**
     * 获取当前时间字符串
     */
    static std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    /**
     * 获取ID
     */
    std::string getId() const {
        return id;
    }

    /**
     * 获取标题
     */
    std::string getTitle() const {
        return title;
    }

    /**
     * 获取内容
     */
    std::string getContent() const {
        return content;
    }

    /**
     * 获取类型
     */
    std::string getType() const {
        return type;
    }

    /**
     * 设置ID
     */
    void setId(const std::string& notificationId) {
        id = notificationId;
    }

    /**
     * 设置标题
     */
    void setTitle(const std::string& notificationTitle) {
        title = notificationTitle;
    }

    /**
     * 设置内容
     */
    void setContent(const std::string& notificationContent) {
        content = notificationContent;
    }

    /**
     * 设置类型
     */
    void setType(const std::string& notificationType) {
        type = notificationType;
    }

    /**
     * 设置发布人信息
     */
    void setPublisher(const std::string& publisherName, const std::string& publisherUserId) {
        publisher = publisherName;
        publisher_id = publisherUserId;
    }

    /**
     * 设置优先级
     */
    void setPriority(int p) {
        priority = p;
    }

    /**
     * 设置置顶
     */
    void setTop(bool top) {
        is_top = top;
    }

    /**
     * 发布通知
     */
    void publish() {
        is_published = true;
        published_at = getCurrentTime();
        updated_at = published_at;
    }

    /**
     * 增加浏览次数
     */
    void incrementViewCount() {
        view_count++;
    }
};

} // namespace chenglei

#endif // NOTIFICATION_HPP

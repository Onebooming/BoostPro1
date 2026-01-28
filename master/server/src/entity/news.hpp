/*
 * @FileName: news.hpp
 * @Description: 学校新闻实体类
 */
#ifndef NEWS_HPP
#define NEWS_HPP

#include <string>
#include <ctime>

namespace chenglei {

class News {
public:
    std::string id;
    std::string title;
    std::string content;
    std::string category;  // 新闻类别（校园新闻、活动通知、获奖喜报等）
    std::string author;
    std::string author_id;
    std::string image_url;
    int view_count;
    bool is_published;
    bool is_featured;
    std::string published_at;
    std::string created_at;
    std::string updated_at;

    News() : view_count(0), is_published(false), is_featured(false) {}

    static std::string getCurrentTime() {
        time_t now = time(0);
        char buf[80];
        struct tm* nowtm = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", nowtm);
        return std::string(buf);
    }
};

} // namespace chenglei
#endif

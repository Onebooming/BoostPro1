/*
 * @FileName: blog.hpp  
 * @Description: 知识博客实体类
 */
#ifndef BLOG_HPP
#define BLOG_HPP

#include <string>
#include <ctime>

namespace chenglei {

class Blog {
public:
    std::string id;
    std::string title;
    std::string content;
    std::string category;  // 博客分类（教学心得、技术分享、学习资料等）
    std::string tags;
    std::string author;
    std::string author_id;
    std::string cover_image;
    int view_count;
    int like_count;
    int comment_count;
    bool is_published;
    bool is_top;
    std::string published_at;
    std::string created_at;
    std::string updated_at;

    Blog() : view_count(0), like_count(0), comment_count(0), is_published(false), is_top(false) {}

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

/*
 * @FileName: blog_dao.hpp
 * @Description: 知识博客数据访问接口
 */
#ifndef BLOG_DAO_HPP
#define BLOG_DAO_HPP

#include "entity/blog.hpp"
#include <vector>
#include <memory>
#include <string>

namespace chenglei {

class BlogDao {
public:
    static BlogDao& getInstance();

    // 表操作
    bool createTable();

    // CRUD操作
    bool insertBlog(const Blog& blog);
    bool updateBlog(const Blog& blog);
    bool deleteBlog(const std::string& id);

    // 查询操作
    std::unique_ptr<Blog> selectBlogById(const std::string& id);
    std::vector<Blog> selectAllBlogs();
    std::vector<Blog> selectPublishedBlogs();
    std::vector<Blog> selectTopBlogs();
    std::vector<Blog> selectBlogsByCategory(const std::string& category);
    std::vector<Blog> selectBlogsByAuthor(const std::string& author_id);
    std::vector<Blog> selectBlogsByTags(const std::string& tags);

    // 统计操作
    int incrementViewCount(const std::string& id);
    int incrementLikeCount(const std::string& id);
    int incrementCommentCount(const std::string& id);

private:
    BlogDao() = default;
    BlogDao(const BlogDao&) = delete;
    BlogDao& operator=(const BlogDao&) = delete;

    std::string escapeSQL(const std::string& input);
};

} // namespace chenglei

#endif // BLOG_DAO_HPP

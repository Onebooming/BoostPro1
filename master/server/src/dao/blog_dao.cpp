/*
 * @FileName: blog_dao.cpp
 * @Description: 知识博客数据访问实现
 */
#include "blog_dao.hpp"
#include "database/mysql_pro.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace chenglei {

BlogDao& BlogDao::getInstance() {
    static BlogDao instance;
    return instance;
}

std::string BlogDao::escapeSQL(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c == '\'' || c == '\\' || c == '"') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

bool BlogDao::createTable() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) {
        std::cerr << "Failed to get MySQL connection" << std::endl;
        return false;
    }
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS blogs (
            id VARCHAR(64) PRIMARY KEY,
            title VARCHAR(200) NOT NULL,
            content TEXT NOT NULL,
            category VARCHAR(50) DEFAULT '教学心得',
            tags VARCHAR(500),
            author VARCHAR(100),
            author_id VARCHAR(64),
            cover_image VARCHAR(500),
            view_count INT DEFAULT 0,
            like_count INT DEFAULT 0,
            comment_count INT DEFAULT 0,
            is_published BOOLEAN DEFAULT FALSE,
            is_top BOOLEAN DEFAULT FALSE,
            published_at DATETIME,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            INDEX idx_category (category),
            INDEX idx_published (is_published),
            INDEX idx_top (is_top),
            INDEX idx_author (author_id),
            FULLTEXT INDEX ft_title_content (title, content, tags)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to create blogs table: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool BlogDao::insertBlog(const Blog& blog) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::stringstream ss;
    ss << "INSERT INTO blogs (id, title, content, category, tags, author, author_id, cover_image, "
       << "view_count, like_count, comment_count, is_published, is_top, published_at) VALUES ('"
       << blog.id << "', '"
       << escapeSQL(blog.title) << "', '"
       << escapeSQL(blog.content) << "', '"
       << escapeSQL(blog.category) << "', '"
       << escapeSQL(blog.tags) << "', '"
       << escapeSQL(blog.author) << "', '"
       << blog.author_id << "', '"
       << escapeSQL(blog.cover_image) << "', "
       << blog.view_count << ", "
       << blog.like_count << ", "
       << blog.comment_count << ", "
       << (blog.is_published ? 1 : 0) << ", "
       << (blog.is_top ? 1 : 0) << ", "
       << (blog.published_at.empty() ? "NULL" : "'" + blog.published_at + "'") << ")";

    if (mysql_query(mysql, ss.str().c_str())) {
        std::cerr << "Failed to insert blog: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool BlogDao::updateBlog(const Blog& blog) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::stringstream ss;
    ss << "UPDATE blogs SET "
       << "title = '" << escapeSQL(blog.title) << "', "
       << "content = '" << escapeSQL(blog.content) << "', "
       << "category = '" << escapeSQL(blog.category) << "', "
       << "tags = '" << escapeSQL(blog.tags) << "', "
       << "author = '" << escapeSQL(blog.author) << "', "
       << "author_id = '" << blog.author_id << "', "
       << "cover_image = '" << escapeSQL(blog.cover_image) << "', "
       << "view_count = " << blog.view_count << ", "
       << "like_count = " << blog.like_count << ", "
       << "comment_count = " << blog.comment_count << ", "
       << "is_published = " << (blog.is_published ? 1 : 0) << ", "
       << "is_top = " << (blog.is_top ? 1 : 0) << ", "
       << "published_at = " << (blog.published_at.empty() ? "NULL" : "'" + blog.published_at + "'") << " "
       << "WHERE id = '" << blog.id << "'";

    if (mysql_query(mysql, ss.str().c_str())) {
        std::cerr << "Failed to update blog: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool BlogDao::deleteBlog(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "DELETE FROM blogs WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to delete blog: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

std::unique_ptr<Blog> BlogDao::selectBlogById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return nullptr;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM blogs WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query blog: " << mysql_error(mysql) << std::endl;
        return nullptr;
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return nullptr;

    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Blog> blog;

    if (row) {
        blog = std::make_unique<Blog>();
        blog->id = row[0] ? row[0] : "";
        blog->title = row[1] ? row[1] : "";
        blog->content = row[2] ? row[2] : "";
        blog->category = row[3] ? row[3] : "教学心得";
        blog->tags = row[4] ? row[4] : "";
        blog->author = row[5] ? row[5] : "";
        blog->author_id = row[6] ? row[6] : "";
        blog->cover_image = row[7] ? row[7] : "";
        blog->view_count = row[8] ? atoi(row[8]) : 0;
        blog->like_count = row[9] ? atoi(row[9]) : 0;
        blog->comment_count = row[10] ? atoi(row[10]) : 0;
        blog->is_published = row[11] ? (atoi(row[11]) != 0) : false;
        blog->is_top = row[12] ? (atoi(row[12]) != 0) : false;
        blog->published_at = row[13] ? row[13] : "";
        blog->created_at = row[14] ? row[14] : "";
        blog->updated_at = row[15] ? row[15] : "";
    }

    mysql_free_result(result);
    return blog;
}

std::vector<Blog> BlogDao::selectAllBlogs() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM blogs ORDER BY created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query blogs: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Blog> blogList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Blog blog;
        blog.id = row[0] ? row[0] : "";
        blog.title = row[1] ? row[1] : "";
        blog.content = row[2] ? row[2] : "";
        blog.category = row[3] ? row[3] : "教学心得";
        blog.tags = row[4] ? row[4] : "";
        blog.author = row[5] ? row[5] : "";
        blog.author_id = row[6] ? row[6] : "";
        blog.cover_image = row[7] ? row[7] : "";
        blog.view_count = row[8] ? atoi(row[8]) : 0;
        blog.like_count = row[9] ? atoi(row[9]) : 0;
        blog.comment_count = row[10] ? atoi(row[10]) : 0;
        blog.is_published = row[11] ? (atoi(row[11]) != 0) : false;
        blog.is_top = row[12] ? (atoi(row[12]) != 0) : false;
        blog.published_at = row[13] ? row[13] : "";
        blog.created_at = row[14] ? row[14] : "";
        blog.updated_at = row[15] ? row[15] : "";

        blogList.push_back(blog);
    }

    mysql_free_result(result);
    return blogList;
}

std::vector<Blog> BlogDao::selectPublishedBlogs() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM blogs WHERE is_published = 1 ORDER BY is_top DESC, published_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query published blogs: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Blog> blogList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Blog blog;
        blog.id = row[0] ? row[0] : "";
        blog.title = row[1] ? row[1] : "";
        blog.content = row[2] ? row[2] : "";
        blog.category = row[3] ? row[3] : "教学心得";
        blog.tags = row[4] ? row[4] : "";
        blog.author = row[5] ? row[5] : "";
        blog.author_id = row[6] ? row[6] : "";
        blog.cover_image = row[7] ? row[7] : "";
        blog.view_count = row[8] ? atoi(row[8]) : 0;
        blog.like_count = row[9] ? atoi(row[9]) : 0;
        blog.comment_count = row[10] ? atoi(row[10]) : 0;
        blog.is_published = row[11] ? (atoi(row[11]) != 0) : false;
        blog.is_top = row[12] ? (atoi(row[12]) != 0) : false;
        blog.published_at = row[13] ? row[13] : "";
        blog.created_at = row[14] ? row[14] : "";
        blog.updated_at = row[15] ? row[15] : "";

        blogList.push_back(blog);
    }

    mysql_free_result(result);
    return blogList;
}

std::vector<Blog> BlogDao::selectTopBlogs() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM blogs WHERE is_top = 1 AND is_published = 1 ORDER BY published_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query top blogs: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Blog> blogList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Blog blog;
        blog.id = row[0] ? row[0] : "";
        blog.title = row[1] ? row[1] : "";
        blog.content = row[2] ? row[2] : "";
        blog.category = row[3] ? row[3] : "教学心得";
        blog.tags = row[4] ? row[4] : "";
        blog.author = row[5] ? row[5] : "";
        blog.author_id = row[6] ? row[6] : "";
        blog.cover_image = row[7] ? row[7] : "";
        blog.view_count = row[8] ? atoi(row[8]) : 0;
        blog.like_count = row[9] ? atoi(row[9]) : 0;
        blog.comment_count = row[10] ? atoi(row[10]) : 0;
        blog.is_published = row[11] ? (atoi(row[11]) != 0) : false;
        blog.is_top = row[12] ? (atoi(row[12]) != 0) : false;
        blog.published_at = row[13] ? row[13] : "";
        blog.created_at = row[14] ? row[14] : "";
        blog.updated_at = row[15] ? row[15] : "";

        blogList.push_back(blog);
    }

    mysql_free_result(result);
    return blogList;
}

std::vector<Blog> BlogDao::selectBlogsByCategory(const std::string& category) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM blogs WHERE category = '" + escapeSQL(category) + "' ORDER BY created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query blogs by category: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Blog> blogList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Blog blog;
        blog.id = row[0] ? row[0] : "";
        blog.title = row[1] ? row[1] : "";
        blog.content = row[2] ? row[2] : "";
        blog.category = row[3] ? row[3] : "教学心得";
        blog.tags = row[4] ? row[4] : "";
        blog.author = row[5] ? row[5] : "";
        blog.author_id = row[6] ? row[6] : "";
        blog.cover_image = row[7] ? row[7] : "";
        blog.view_count = row[8] ? atoi(row[8]) : 0;
        blog.like_count = row[9] ? atoi(row[9]) : 0;
        blog.comment_count = row[10] ? atoi(row[10]) : 0;
        blog.is_published = row[11] ? (atoi(row[11]) != 0) : false;
        blog.is_top = row[12] ? (atoi(row[12]) != 0) : false;
        blog.published_at = row[13] ? row[13] : "";
        blog.created_at = row[14] ? row[14] : "";
        blog.updated_at = row[15] ? row[15] : "";

        blogList.push_back(blog);
    }

    mysql_free_result(result);
    return blogList;
}

std::vector<Blog> BlogDao::selectBlogsByAuthor(const std::string& author_id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM blogs WHERE author_id = '" + escapeSQL(author_id) + "' ORDER BY created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query blogs by author: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Blog> blogList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Blog blog;
        blog.id = row[0] ? row[0] : "";
        blog.title = row[1] ? row[1] : "";
        blog.content = row[2] ? row[2] : "";
        blog.category = row[3] ? row[3] : "教学心得";
        blog.tags = row[4] ? row[4] : "";
        blog.author = row[5] ? row[5] : "";
        blog.author_id = row[6] ? row[6] : "";
        blog.cover_image = row[7] ? row[7] : "";
        blog.view_count = row[8] ? atoi(row[8]) : 0;
        blog.like_count = row[9] ? atoi(row[9]) : 0;
        blog.comment_count = row[10] ? atoi(row[10]) : 0;
        blog.is_published = row[11] ? (atoi(row[11]) != 0) : false;
        blog.is_top = row[12] ? (atoi(row[12]) != 0) : false;
        blog.published_at = row[13] ? row[13] : "";
        blog.created_at = row[14] ? row[14] : "";
        blog.updated_at = row[15] ? row[15] : "";

        blogList.push_back(blog);
    }

    mysql_free_result(result);
    return blogList;
}

std::vector<Blog> BlogDao::selectBlogsByTags(const std::string& tags) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM blogs WHERE tags LIKE '%" + escapeSQL(tags) + "%' ORDER BY created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query blogs by tags: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Blog> blogList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Blog blog;
        blog.id = row[0] ? row[0] : "";
        blog.title = row[1] ? row[1] : "";
        blog.content = row[2] ? row[2] : "";
        blog.category = row[3] ? row[3] : "教学心得";
        blog.tags = row[4] ? row[4] : "";
        blog.author = row[5] ? row[5] : "";
        blog.author_id = row[6] ? row[6] : "";
        blog.cover_image = row[7] ? row[7] : "";
        blog.view_count = row[8] ? atoi(row[8]) : 0;
        blog.like_count = row[9] ? atoi(row[9]) : 0;
        blog.comment_count = row[10] ? atoi(row[10]) : 0;
        blog.is_published = row[11] ? (atoi(row[11]) != 0) : false;
        blog.is_top = row[12] ? (atoi(row[12]) != 0) : false;
        blog.published_at = row[13] ? row[13] : "";
        blog.created_at = row[14] ? row[14] : "";
        blog.updated_at = row[15] ? row[15] : "";

        blogList.push_back(blog);
    }

    mysql_free_result(result);
    return blogList;
}

int BlogDao::incrementViewCount(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return -1;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "UPDATE blogs SET view_count = view_count + 1 WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to increment view count: " << mysql_error(mysql) << std::endl;
        return -1;
    }

    return mysql_affected_rows(mysql);
}

int BlogDao::incrementLikeCount(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return -1;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "UPDATE blogs SET like_count = like_count + 1 WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to increment like count: " << mysql_error(mysql) << std::endl;
        return -1;
    }

    return mysql_affected_rows(mysql);
}

int BlogDao::incrementCommentCount(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return -1;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "UPDATE blogs SET comment_count = comment_count + 1 WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to increment comment count: " << mysql_error(mysql) << std::endl;
        return -1;
    }

    return mysql_affected_rows(mysql);
}

} // namespace chenglei

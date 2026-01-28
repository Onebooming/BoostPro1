/*
 * @FileName: news_dao.cpp
 * @Description: 学校新闻数据访问实现
 */
#include "news_dao.hpp"
#include "database/mysql_pro.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace chenglei {

NewsDao& NewsDao::getInstance() {
    static NewsDao instance;
    return instance;
}

std::string NewsDao::escapeSQL(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c == '\'' || c == '\\' || c == '"') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

bool NewsDao::createTable() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) {
        std::cerr << "Failed to get MySQL connection" << std::endl;
        return false;
    }
    MYSQL* mysql = conn_client->get();

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS news (
            id VARCHAR(64) PRIMARY KEY,
            title VARCHAR(200) NOT NULL,
            content TEXT NOT NULL,
            category VARCHAR(50) DEFAULT '校园新闻',
            author VARCHAR(100),
            author_id VARCHAR(64),
            image_url VARCHAR(500),
            view_count INT DEFAULT 0,
            is_published BOOLEAN DEFAULT FALSE,
            is_featured BOOLEAN DEFAULT FALSE,
            published_at DATETIME,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            INDEX idx_category (category),
            INDEX idx_published (is_published),
            INDEX idx_featured (is_featured),
            INDEX idx_author (author_id),
            FULLTEXT INDEX ft_title_content (title, content)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to create news table: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool NewsDao::insertNews(const News& news) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;

    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::stringstream ss;
    ss << "INSERT INTO news (id, title, content, category, author, author_id, image_url, "
       << "view_count, is_published, is_featured, published_at) VALUES ('"
       << news.id << "', '"
       << escapeSQL(news.title) << "', '"
       << escapeSQL(news.content) << "', '"
       << escapeSQL(news.category) << "', '"
       << escapeSQL(news.author) << "', '"
       << news.author_id << "', '"
       << escapeSQL(news.image_url) << "', "
       << news.view_count << ", "
       << (news.is_published ? 1 : 0) << ", "
       << (news.is_featured ? 1 : 0) << ", "
       << (news.published_at.empty() ? "NULL" : "'" + news.published_at + "'") << ")";

    if (mysql_query(mysql, ss.str().c_str())) {
        std::cerr << "Failed to insert news: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool NewsDao::updateNews(const News& news) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    MYSQL* mysql = conn_client->get();

    std::stringstream ss;
    ss << "UPDATE news SET "
       << "title = '" << escapeSQL(news.title) << "', "
       << "content = '" << escapeSQL(news.content) << "', "
       << "category = '" << escapeSQL(news.category) << "', "
       << "author = '" << escapeSQL(news.author) << "', "
       << "author_id = '" << news.author_id << "', "
       << "image_url = '" << escapeSQL(news.image_url) << "', "
       << "view_count = " << news.view_count << ", "
       << "is_published = " << (news.is_published ? 1 : 0) << ", "
       << "is_featured = " << (news.is_featured ? 1 : 0) << ", "
       << "published_at = " << (news.published_at.empty() ? "NULL" : "'" + news.published_at + "'") << " "
       << "WHERE id = '" << news.id << "'";

    if (mysql_query(mysql, ss.str().c_str())) {
        std::cerr << "Failed to update news: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool NewsDao::deleteNews(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    MYSQL* mysql = conn_client->get();

    std::string sql = "DELETE FROM news WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to delete news: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

std::unique_ptr<News> NewsDao::selectNewsById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return nullptr;
    MYSQL* mysql = conn_client->get();

    std::string sql = "SELECT * FROM news WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query news: " << mysql_error(mysql) << std::endl;
        return nullptr;
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return nullptr;

    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<News> news;

    if (row) {
        news = std::make_unique<News>();
        news->id = row[0] ? row[0] : "";
        news->title = row[1] ? row[1] : "";
        news->content = row[2] ? row[2] : "";
        news->category = row[3] ? row[3] : "校园新闻";
        news->author = row[4] ? row[4] : "";
        news->author_id = row[5] ? row[5] : "";
        news->image_url = row[6] ? row[6] : "";
        news->view_count = row[7] ? atoi(row[7]) : 0;
        news->is_published = row[8] ? (atoi(row[8]) != 0) : false;
        news->is_featured = row[9] ? (atoi(row[9]) != 0) : false;
        news->published_at = row[10] ? row[10] : "";
        news->created_at = row[11] ? row[11] : "";
        news->updated_at = row[12] ? row[12] : "";
    }

    mysql_free_result(result);
    return news;
}

std::vector<News> NewsDao::selectAllNews() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    MYSQL* mysql = conn_client->get();

    std::string sql = "SELECT * FROM news ORDER BY created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query news: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<News> newsList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        News news;
        news.id = row[0] ? row[0] : "";
        news.title = row[1] ? row[1] : "";
        news.content = row[2] ? row[2] : "";
        news.category = row[3] ? row[3] : "校园新闻";
        news.author = row[4] ? row[4] : "";
        news.author_id = row[5] ? row[5] : "";
        news.image_url = row[6] ? row[6] : "";
        news.view_count = row[7] ? atoi(row[7]) : 0;
        news.is_published = row[8] ? (atoi(row[8]) != 0) : false;
        news.is_featured = row[9] ? (atoi(row[9]) != 0) : false;
        news.published_at = row[10] ? row[10] : "";
        news.created_at = row[11] ? row[11] : "";
        news.updated_at = row[12] ? row[12] : "";

        newsList.push_back(news);
    }

    mysql_free_result(result);
    return newsList;
}

std::vector<News> NewsDao::selectPublishedNews() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    MYSQL* mysql = conn_client->get();

    std::string sql = "SELECT * FROM news WHERE is_published = 1 ORDER BY published_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query published news: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<News> newsList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        News news;
        news.id = row[0] ? row[0] : "";
        news.title = row[1] ? row[1] : "";
        news.content = row[2] ? row[2] : "";
        news.category = row[3] ? row[3] : "校园新闻";
        news.author = row[4] ? row[4] : "";
        news.author_id = row[5] ? row[5] : "";
        news.image_url = row[6] ? row[6] : "";
        news.view_count = row[7] ? atoi(row[7]) : 0;
        news.is_published = row[8] ? (atoi(row[8]) != 0) : false;
        news.is_featured = row[9] ? (atoi(row[9]) != 0) : false;
        news.published_at = row[10] ? row[10] : "";
        news.created_at = row[11] ? row[11] : "";
        news.updated_at = row[12] ? row[12] : "";

        newsList.push_back(news);
    }

    mysql_free_result(result);
    return newsList;
}

std::vector<News> NewsDao::selectFeaturedNews() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    MYSQL* mysql = conn_client->get();

    std::string sql = "SELECT * FROM news WHERE is_featured = 1 AND is_published = 1 ORDER BY published_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query featured news: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<News> newsList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        News news;
        news.id = row[0] ? row[0] : "";
        news.title = row[1] ? row[1] : "";
        news.content = row[2] ? row[2] : "";
        news.category = row[3] ? row[3] : "校园新闻";
        news.author = row[4] ? row[4] : "";
        news.author_id = row[5] ? row[5] : "";
        news.image_url = row[6] ? row[6] : "";
        news.view_count = row[7] ? atoi(row[7]) : 0;
        news.is_published = row[8] ? (atoi(row[8]) != 0) : false;
        news.is_featured = row[9] ? (atoi(row[9]) != 0) : false;
        news.published_at = row[10] ? row[10] : "";
        news.created_at = row[11] ? row[11] : "";
        news.updated_at = row[12] ? row[12] : "";

        newsList.push_back(news);
    }

    mysql_free_result(result);
    return newsList;
}

std::vector<News> NewsDao::selectNewsByCategory(const std::string& category) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    MYSQL* mysql = conn_client->get();

    std::string sql = "SELECT * FROM news WHERE category = '" + escapeSQL(category) + "' ORDER BY created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query news by category: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<News> newsList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        News news;
        news.id = row[0] ? row[0] : "";
        news.title = row[1] ? row[1] : "";
        news.content = row[2] ? row[2] : "";
        news.category = row[3] ? row[3] : "校园新闻";
        news.author = row[4] ? row[4] : "";
        news.author_id = row[5] ? row[5] : "";
        news.image_url = row[6] ? row[6] : "";
        news.view_count = row[7] ? atoi(row[7]) : 0;
        news.is_published = row[8] ? (atoi(row[8]) != 0) : false;
        news.is_featured = row[9] ? (atoi(row[9]) != 0) : false;
        news.published_at = row[10] ? row[10] : "";
        news.created_at = row[11] ? row[11] : "";
        news.updated_at = row[12] ? row[12] : "";

        newsList.push_back(news);
    }

    mysql_free_result(result);
    return newsList;
}

std::vector<News> NewsDao::selectNewsByAuthor(const std::string& author_id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    MYSQL* mysql = conn_client->get();

    std::string sql = "SELECT * FROM news WHERE author_id = '" + escapeSQL(author_id) + "' ORDER BY created_at DESC";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query news by author: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<News> newsList;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        News news;
        news.id = row[0] ? row[0] : "";
        news.title = row[1] ? row[1] : "";
        news.content = row[2] ? row[2] : "";
        news.category = row[3] ? row[3] : "校园新闻";
        news.author = row[4] ? row[4] : "";
        news.author_id = row[5] ? row[5] : "";
        news.image_url = row[6] ? row[6] : "";
        news.view_count = row[7] ? atoi(row[7]) : 0;
        news.is_published = row[8] ? (atoi(row[8]) != 0) : false;
        news.is_featured = row[9] ? (atoi(row[9]) != 0) : false;
        news.published_at = row[10] ? row[10] : "";
        news.created_at = row[11] ? row[11] : "";
        news.updated_at = row[12] ? row[12] : "";

        newsList.push_back(news);
    }

    mysql_free_result(result);
    return newsList;
}

int NewsDao::incrementViewCount(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return -1;
    MYSQL* mysql = conn_client->get();

    std::string sql = "UPDATE news SET view_count = view_count + 1 WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to increment view count: " << mysql_error(mysql) << std::endl;
        return -1;
    }

    return mysql_affected_rows(mysql);
}

} // namespace chenglei

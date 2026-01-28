/*
 * @FileName: news_dao.hpp
 * @Description: 学校新闻数据访问接口
 */
#ifndef NEWS_DAO_HPP
#define NEWS_DAO_HPP

#include "entity/news.hpp"
#include <vector>
#include <memory>
#include <string>

namespace chenglei {

class NewsDao {
public:
    static NewsDao& getInstance();

    // 表操作
    bool createTable();

    // CRUD操作
    bool insertNews(const News& news);
    bool updateNews(const News& news);
    bool deleteNews(const std::string& id);

    // 查询操作
    std::unique_ptr<News> selectNewsById(const std::string& id);
    std::vector<News> selectAllNews();
    std::vector<News> selectPublishedNews();
    std::vector<News> selectFeaturedNews();
    std::vector<News> selectNewsByCategory(const std::string& category);
    std::vector<News> selectNewsByAuthor(const std::string& author_id);

    // 统计操作
    int incrementViewCount(const std::string& id);

private:
    NewsDao() = default;
    NewsDao(const NewsDao&) = delete;
    NewsDao& operator=(const NewsDao&) = delete;

    std::string escapeSQL(const std::string& input);
};

} // namespace chenglei

#endif // NEWS_DAO_HPP

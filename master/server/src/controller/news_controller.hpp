/*
 * @FileName: news_controller.hpp
 * @Description: 学校新闻控制器
 */
#ifndef NEWS_CONTROLLER_HPP
#define NEWS_CONTROLLER_HPP

#include "base_controller.hpp"
#include "dao/news_dao.hpp"
#include "utils/snowflake.hpp"
#include "../../../public/json.hpp"
#include <boost/beast/http.hpp>
#include <string>
#include <memory>

namespace http = boost::beast::http;
using json = nlohmann::json;

namespace chenglei {

class NewsController : public BaseController {
private:
    NewsDao& newsDao = NewsDao::getInstance();

public:
    NewsController() {
        // 懒加载表创建
    }

    int controller_process(http::request<http::string_body>& request, http::response<http::string_body>& response) override {
        // 确保表存在（懒加载）
        static bool tableCreated = false;
        if (!tableCreated) {
            newsDao.createTable();
            tableCreated = true;
        }

        std::string url = request.target().to_string();
        std::string action = getQueryParamFromUrl(url, "action");

        // If action is not in URL and it's a POST request, try to get it from body
        if (action.empty() && request.method() == http::verb::post) {
            try {
                json body = json::parse(request.body());
                if (body.contains("action")) {
                    action = body["action"];
                }
            } catch (...) {
                // If parsing fails, action remains empty
            }
        }

        if (action.empty() && request.method() == http::verb::get) {
            action = "list_news";
        }

        try {
            if (action == "list_news") {
                return listNews(response);
            } else if (action == "query_by_id") {
                return queryById(request, response);
            } else if (action == "add_news") {
                return addNews(request, response);
            } else if (action == "update_news") {
                return updateNews(request, response);
            } else if (action == "delete_news") {
                return deleteNews(request, response);
            } else if (action == "list_published") {
                return listPublished(response);
            } else if (action == "list_featured") {
                return listFeatured(response);
            } else if (action == "list_by_category") {
                return listByCategory(request, response);
            } else if (action == "list_by_author") {
                return listByAuthor(request, response);
            } else if (action == "publish_news") {
                return publishNews(request, response);
            } else if (action == "increment_view") {
                return incrementView(request, response);
            } else {
                return errorResponse(response, "Unknown action: " + action, 400);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

private:
    int listNews(http::response<http::string_body>& response) {
        auto newsList = newsDao.selectAllNews();

        json jsonArray = json::array();
        for (const auto& news : newsList) {
            jsonArray.push_back(newsToJson(news));
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = jsonArray;

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int queryById(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        std::string url = request.target().to_string();
        std::string id = getQueryParamFromUrl(url, "id");

        if (id.empty()) {
            return errorResponse(response, "Missing id parameter", 400);
        }

        auto news = newsDao.selectNewsById(id);
        if (!news) {
            return errorResponse(response, "News not found", 404);
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = newsToJson(*news);

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int addNews(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        try {
            json data = json::parse(request.body());

            // 验证必填字段
            if (!data.contains("title") || !data.contains("content")) {
                return errorResponse(response, "Missing required fields: title and content", 400);
            }

            News news;
            news.id = SnowflakeIDGenerator::instance().next_id();
            news.title = data["title"];
            news.content = data["content"];
            news.category = data.value("category", "校园新闻");
            news.author = data.value("author", "");
            news.author_id = data.value("author_id", "");
            news.image_url = data.value("image_url", "");
            news.view_count = 0;
            news.is_published = data.value("is_published", false);
            news.is_featured = data.value("is_featured", false);
            news.published_at = data.value("published_at", "");

            if (newsDao.insertNews(news)) {
                json jsonResponse;
                jsonResponse["code"] = 200;
                jsonResponse["message"] = "News added successfully";
                jsonResponse["data"] = newsToJson(news);
                return buildJsonResponse(response, jsonResponse, 200);
            } else {
                return errorResponse(response, "Failed to add news", 500);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

    int updateNews(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        try {
            json data = json::parse(request.body());

            if (!data.contains("id")) {
                return errorResponse(response, "Missing id field", 400);
            }

            auto existingNews = newsDao.selectNewsById(data["id"]);
            if (!existingNews) {
                return errorResponse(response, "News not found", 404);
            }

            // 更新字段
            if (data.contains("title")) existingNews->title = data["title"];
            if (data.contains("content")) existingNews->content = data["content"];
            if (data.contains("category")) existingNews->category = data["category"];
            if (data.contains("author")) existingNews->author = data["author"];
            if (data.contains("author_id")) existingNews->author_id = data["author_id"];
            if (data.contains("image_url")) existingNews->image_url = data["image_url"];
            if (data.contains("view_count")) existingNews->view_count = data["view_count"];
            if (data.contains("is_published")) existingNews->is_published = data["is_published"];
            if (data.contains("is_featured")) existingNews->is_featured = data["is_featured"];
            if (data.contains("published_at")) existingNews->published_at = data["published_at"];

            if (newsDao.updateNews(*existingNews)) {
                json jsonResponse;
                jsonResponse["code"] = 200;
                jsonResponse["message"] = "News updated successfully";
                jsonResponse["data"] = newsToJson(*existingNews);
                return buildJsonResponse(response, jsonResponse, 200);
            } else {
                return errorResponse(response, "Failed to update news", 500);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

    int deleteNews(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        try {
            json data = json::parse(request.body());

            if (!data.contains("id")) {
                return errorResponse(response, "Missing id field", 400);
            }

            if (newsDao.deleteNews(data["id"])) {
                json jsonResponse;
                jsonResponse["code"] = 200;
                jsonResponse["message"] = "News deleted successfully";
                return buildJsonResponse(response, jsonResponse, 200);
            } else {
                return errorResponse(response, "Failed to delete news", 500);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

    int listPublished(http::response<http::string_body>& response) {
        auto newsList = newsDao.selectPublishedNews();

        json jsonArray = json::array();
        for (const auto& news : newsList) {
            jsonArray.push_back(newsToJson(news));
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = jsonArray;

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int listFeatured(http::response<http::string_body>& response) {
        auto newsList = newsDao.selectFeaturedNews();

        json jsonArray = json::array();
        for (const auto& news : newsList) {
            jsonArray.push_back(newsToJson(news));
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = jsonArray;

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int listByCategory(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        std::string url = request.target().to_string();
        std::string category = getQueryParamFromUrl(url, "category");

        if (category.empty()) {
            return errorResponse(response, "Missing category parameter", 400);
        }

        auto newsList = newsDao.selectNewsByCategory(category);

        json jsonArray = json::array();
        for (const auto& news : newsList) {
            jsonArray.push_back(newsToJson(news));
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = jsonArray;

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int listByAuthor(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        std::string url = request.target().to_string();
        std::string author_id = getQueryParamFromUrl(url, "author_id");

        if (author_id.empty()) {
            return errorResponse(response, "Missing author_id parameter", 400);
        }

        auto newsList = newsDao.selectNewsByAuthor(author_id);

        json jsonArray = json::array();
        for (const auto& news : newsList) {
            jsonArray.push_back(newsToJson(news));
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = jsonArray;

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int publishNews(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        try {
            json data = json::parse(request.body());

            if (!data.contains("id")) {
                return errorResponse(response, "Missing id field", 400);
            }

            auto existingNews = newsDao.selectNewsById(data["id"]);
            if (!existingNews) {
                return errorResponse(response, "News not found", 404);
            }

            existingNews->is_published = true;
            if (existingNews->published_at.empty()) {
                existingNews->published_at = News::getCurrentTime();
            }

            if (newsDao.updateNews(*existingNews)) {
                json jsonResponse;
                jsonResponse["code"] = 200;
                jsonResponse["message"] = "News published successfully";
                jsonResponse["data"] = newsToJson(*existingNews);
                return buildJsonResponse(response, jsonResponse, 200);
            } else {
                return errorResponse(response, "Failed to publish news", 500);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

    int incrementView(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        try {
            json data = json::parse(request.body());

            if (!data.contains("id")) {
                return errorResponse(response, "Missing id field", 400);
            }

            int result = newsDao.incrementViewCount(data["id"]);

            if (result >= 0) {
                json jsonResponse;
                jsonResponse["code"] = 200;
                jsonResponse["message"] = "View count incremented successfully";
                return buildJsonResponse(response, jsonResponse, 200);
            } else {
                return errorResponse(response, "Failed to increment view count", 500);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

    json newsToJson(const News& news) {
        json j;
        j["id"] = news.id;
        j["title"] = news.title;
        j["content"] = news.content;
        j["category"] = news.category;
        j["author"] = news.author;
        j["author_id"] = news.author_id;
        j["image_url"] = news.image_url;
        j["view_count"] = news.view_count;
        j["is_published"] = news.is_published;
        j["is_featured"] = news.is_featured;
        j["published_at"] = news.published_at;
        j["created_at"] = news.created_at;
        j["updated_at"] = news.updated_at;
        return j;
    }

    int buildJsonResponse(http::response<http::string_body>& response, const json& j, int status) {
        response.result(status);
        response.set(http::field::server, "Boost.Beast");
        response.set(http::field::content_type, "application/json");
        response.body() = j.dump();
        response.content_length(response.body().size());
        return 0;
    }

    int errorResponse(http::response<http::string_body>& response, const std::string& message, int status) {
        json j;
        j["code"] = status;
        j["error"] = message;
        return buildJsonResponse(response, j, status);
    }

    std::string getQueryParamFromUrl(const std::string& url, const std::string& paramName) {
        size_t pos = url.find("?" + paramName + "=");
        if (pos == std::string::npos) {
            pos = url.find("&" + paramName + "=");
        }
        if (pos != std::string::npos) {
            size_t start = pos + paramName.length() + 2; // +2 for "?=" or "&="
            size_t end = url.find("&", start);
            if (end == std::string::npos) {
                return url.substr(start);
            }
            return url.substr(start, end - start);
        }
        return "";
    }
};

} // namespace chenglei

#endif // NEWS_CONTROLLER_HPP

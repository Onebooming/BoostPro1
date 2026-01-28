#pragma once
#include "../utils/restful_response.hpp"
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <unordered_map>

namespace chenglei {

namespace http = boost::beast::http;

// 查询参数解析器
class QueryParams {
public:
    QueryParams(const std::string& query_string) {
        if (query_string.empty()) return;

        std::istringstream iss(query_string);
        std::string param;
        while (std::getline(iss, param, '&')) {
            size_t eq_pos = param.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = param.substr(0, eq_pos);
                std::string value = param.substr(eq_pos + 1);
                params_[key] = value;
            }
        }
    }

    std::string get(const std::string& key, const std::string& default_value = "") const {
        auto it = params_.find(key);
        return it != params_.end() ? it->second : default_value;
    }

    int getInt(const std::string& key, int default_value = 0) const {
        std::string value = get(key);
        return value.empty() ? default_value : std::stoi(value);
    }

    bool has(const std::string& key) const {
        return params_.find(key) != params_.end();
    }

    // 分页参数
    struct PageInfo {
        int page;
        int page_size;

        static PageInfo fromQuery(const QueryParams& params) {
            int page = params.getInt("page", 1);
            int page_size = params.getInt("page_size", 20);

            // 限制范围
            if (page < 1) page = 1;
            if (page_size < 1) page_size = 20;
            if (page_size > 100) page_size = 100;

            return {page, page_size};
        }
    };

private:
    std::unordered_map<std::string, std::string> params_;
};

// RESTful API 控制器基类
class RestfulController {
public:
    virtual ~RestfulController() = default;

    // 主入口 - 根据HTTP方法分发
    http::response<http::string_body> handleRequest(
        const http::request<http::string_body>& req,
        const std::string& resource_id = ""
    ) {
        try {
            switch (req.method()) {
                case http::verb::get:
                    return resource_id.empty() ? handleList(req) : handleGet(req, resource_id);
                case http::verb::post:
                    return handlePost(req);
                case http::verb::put:
                    return handlePut(req, resource_id);
                case http::verb::patch:
                    return handlePatch(req, resource_id);
                case http::verb::delete_:
                    return handleDelete(req, resource_id);
                case http::verb::options:
                    return handleOptions();
                default:
                    return methodNotAllowed();
            }
        } catch (const std::exception& e) {
            return RestfulResponse::internalError(e.what());
        }
    }

protected:
    // 子类需要实现的方法
    virtual http::response<http::string_body> handleList(
        const http::request<http::string_body>& req) = 0;

    virtual http::response<http::string_body> handleGet(
        const http::request<http::string_body>& req,
        const std::string& id) = 0;

    virtual http::response<http::string_body> handlePost(
        const http::request<http::string_body>& req) = 0;

    virtual http::response<http::string_body> handlePut(
        const http::request<http::string_body>& req,
        const std::string& id) {
        return RestfulResponse::error(
            HttpStatus::UNPROCESSABLE_ENTITY,
            "PUT method not implemented",
            "NotImplemented",
            "Use PATCH instead for partial updates"
        );
    }

    virtual http::response<http::string_body> handlePatch(
        const http::request<http::string_body>& req,
        const std::string& id) {
        return RestfulResponse::error(
            HttpStatus::UNPROCESSABLE_ENTITY,
            "PATCH method not implemented",
            "NotImplemented",
            "Use PUT instead for full updates"
        );
    }

    virtual http::response<http::string_body> handleDelete(
        const http::request<http::string_body>& req,
        const std::string& id) = 0;

    // 工具方法
    QueryParams parseQueryParams(const http::request<http::string_body>& req) {
        std::string target = std::string(req.target());
        size_t query_start = target.find('?');
        if (query_start == std::string::npos) {
            return QueryParams("");
        }
        return QueryParams(target.substr(query_start + 1));
    }

    nlohmann::json parseJsonBody(const http::request<http::string_body>& req) {
        try {
            std::string body = req.body();
            if (body.empty()) {
                return nlohmann::json::object();
            }
            return nlohmann::json::parse(body);
        } catch (const nlohmann::json::parse_error& e) {
            throw std::runtime_error("Invalid JSON: " + std::string(e.what()));
        }
    }

private:
    http::response<http::string_body> handleOptions() {
        http::response<http::string_body> res{http::status::ok, 11};
        res.set(http::field::server, "BoostPro Server");
        res.set(http::field::access_control_allow_origin, "*");
        res.set(http::field::access_control_allow_methods, "GET, POST, PUT, PATCH, DELETE, OPTIONS");
        res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
        res.set(http::field::access_control_max_age, "86400");
        res.content_length(0);
        return res;
    }

    http::response<http::string_body> methodNotAllowed() {
        return RestfulResponse::error(
            HttpStatus::UNPROCESSABLE_ENTITY,
            "Method Not Allowed",
            "MethodNotAllowed",
            "The HTTP method is not allowed for this endpoint"
        );
    }
};

} // namespace chenglei

#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <boost/beast/http.hpp>
#include <chrono>

namespace chenglei {

namespace http = boost::beast::http;

// HTTP状态码枚举
enum class HttpStatus {
    OK = 200,
    CREATED = 201,
    NO_CONTENT = 204,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    CONFLICT = 409,
    UNPROCESSABLE_ENTITY = 422,
    TOO_MANY_REQUESTS = 429,
    INTERNAL_SERVER_ERROR = 500
};

// 分页信息
struct Pagination {
    int page;
    int page_size;
    int total;
    int total_pages;

    nlohmann::json toJson() const {
        return {
            {"page", page},
            {"page_size", page_size},
            {"total", total},
            {"total_pages", total_pages}
        };
    }

    static Pagination create(int page, int page_size, int total) {
        int total_pages = (total + page_size - 1) / page_size;
        return {page, page_size, total, total_pages};
    }
};

// RESTful响应类
class RestfulResponse {
public:
    // 成功响应（带数据）
    static http::response<http::string_body> success(
        const nlohmann::json& data,
        const std::string& message = "success"
    ) {
        nlohmann::json response_body = {
            {"code", 200},
            {"message", message},
            {"data", data},
            {"timestamp", getCurrentTimestamp()}
        };
        return buildResponse(HttpStatus::OK, response_body);
    }

    // 成功响应（带分页）
    static http::response<http::string_body> successWithPagination(
        const nlohmann::json& items,
        const Pagination& pagination,
        const std::string& message = "success"
    ) {
        nlohmann::json response_body = {
            {"code", 200},
            {"message", message},
            {"data", {
                {"items", items},
                {"pagination", pagination.toJson()}
            }},
            {"timestamp", getCurrentTimestamp()}
        };
        return buildResponse(HttpStatus::OK, response_body);
    }

    // 创建成功
    static http::response<http::string_body> created(
        const nlohmann::json& data,
        const std::string& message = "Resource created successfully"
    ) {
        nlohmann::json response_body = {
            {"code", 201},
            {"message", message},
            {"data", data},
            {"timestamp", getCurrentTimestamp()}
        };
        return buildResponse(HttpStatus::CREATED, response_body);
    }

    // 无内容（删除成功）
    static http::response<http::string_body> noContent() {
        http::response<http::string_body> res{HttpStatus::NO_CONTENT, 11};
        res.set(http::field::server, "BoostPro Server");
        res.set(http::field::content_type, "application/json");
        return res;
    }

    // 错误响应
    static http::response<http::string_body> error(
        HttpStatus status,
        const std::string& message,
        const std::string& error_type = "",
        const std::string& details = ""
    ) {
        nlohmann::json error_obj = {
            {"type", error_type},
            {"details", details}
        };

        nlohmann::json response_body = {
            {"code", static_cast<int>(status)},
            {"message", message},
            {"error", error_obj},
            {"timestamp", getCurrentTimestamp()}
        };
        return buildResponse(status, response_body);
    }

    // 验证错误
    static http::response<http::string_body> validationError(
        const nlohmann::json& validation_errors
    ) {
        nlohmann::json response_body = {
            {"code", 400},
            {"message", "Validation Error"},
            {"error", {
                {"type", "ValidationError"},
                {"details", validation_errors}
            }},
            {"timestamp", getCurrentTimestamp()}
        };
        return buildResponse(HttpStatus::BAD_REQUEST, response_body);
    }

    // 未找到
    static http::response<http::string_body> notFound(
        const std::string& resource = "Resource"
    ) {
        return error(
            HttpStatus::NOT_FOUND,
            resource + " Not Found",
            "NotFoundError",
            "The requested " + resource + " does not exist"
        );
    }

    // 冲突（重复）
    static http::response<http::string_body> conflict(
        const std::string& field,
        const std::string& value
    ) {
        nlohmann::json error_obj = {
            {"type", "DuplicateError"},
            {"field", field},
            {"value", value}
        };

        nlohmann::json response_body = {
            {"code", 409},
            {"message", "Duplicate " + field},
            {"error", error_obj},
            {"timestamp", getCurrentTimestamp()}
        };
        return buildResponse(HttpStatus::CONFLICT, response_body);
    }

    // 未授权
    static http::response<http::string_body> unauthorized(
        const std::string& message = "Authentication required"
    ) {
        return error(
            HttpStatus::UNAUTHORIZED,
            message,
            "UnauthorizedError",
            "You need to provide valid authentication credentials"
        );
    }

    // 禁止访问
    static http::response<http::string_body> forbidden(
        const std::string& message = "Access denied"
    ) {
        return error(
            HttpStatus::FORBIDDEN,
            message,
            "ForbiddenError",
            "You don't have permission to access this resource"
        );
    }

    // 服务器错误
    static http::response<http::string_body> internalError(
        const std::string& message = "Internal server error"
    ) {
        return error(
            HttpStatus::INTERNAL_SERVER_ERROR,
            message,
            "InternalServerError",
            "An unexpected error occurred"
        );
    }

private:
    static http::response<http::string_body> buildResponse(
        HttpStatus status,
        const nlohmann::json& body
    ) {
        http::response<http::string_body> res{
            static_cast<unsigned>(status), 11
        };

        res.set(http::field::server, "BoostPro Server");
        res.set(http::field::content_type, "application/json");
        res.set(http::field::access_control_allow_origin, "*");
        res.set(http::field::access_control_allow_methods, "GET, POST, PUT, PATCH, DELETE, OPTIONS");
        res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");

        res.body() = body.dump(2);  // 美化JSON输出
        res.content_length(res.body().size());
        res.prepare_payload();

        return res;
    }

    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }
};

} // namespace chenglei

#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace chenglei {

// API响应基类
class ApiResponse {
public:
    enum class Status {
        SUCCESS = 200,
        CREATED = 201,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        CONFLICT = 409,
        INTERNAL_ERROR = 500
    };

    ApiResponse(Status status, const std::string& message = "");
    virtual ~ApiResponse() = default;

    nlohmann::json toJson() const;

    Status getStatus() const { return status_; }
    const std::string& getMessage() const { return message_; }

protected:
    Status status_;
    std::string message_;
};

// 成功响应（带数据）
class SuccessResponse : public ApiResponse {
public:
    template<typename T>
    SuccessResponse(const T& data, const std::string& message = "操作成功")
        : ApiResponse(Status::SUCCESS, message), data_(nlohmann::json(data)) {}

    SuccessResponse(const nlohmann::json& data, const std::string& message = "操作成功");

    nlohmann::json toJson() const;

private:
    nlohmann::json data_;
};

// 错误响应
class ErrorResponse : public ApiResponse {
public:
    ErrorResponse(Status status, const std::string& message, const std::string& error_code = "");

    const std::string& getErrorCode() const { return error_code_; }

    nlohmann::json toJson() const;

private:
    std::string error_code_;
};

// 分页响应
class PaginatedResponse : public ApiResponse {
public:
    template<typename T>
    PaginatedResponse(const std::vector<T>& items, int total, int page, int page_size,
                     const std::string& message = "查询成功")
        : ApiResponse(Status::SUCCESS, message), total_(total), page_(page), page_size_(page_size) {

        nlohmann::json items_json = nlohmann::json::array();
        for (const auto& item : items) {
            items_json.push_back(nlohmann::json(item));
        }
        items_ = items_json;
    }

    nlohmann::json toJson() const;

private:
    nlohmann::json items_;
    int total_;
    int page_;
    int page_size_;
};

// 响应构建器
class ResponseBuilder {
public:
    static SuccessResponse success(const nlohmann::json& data, const std::string& message = "操作成功");
    static SuccessResponse success(const std::string& message = "操作成功");

    static ErrorResponse badRequest(const std::string& message, const std::string& error_code = "BAD_REQUEST");
    static ErrorResponse notFound(const std::string& message = "资源未找到", const std::string& error_code = "NOT_FOUND");
    static ErrorResponse conflict(const std::string& message = "资源冲突", const std::string& error_code = "CONFLICT");
    static ErrorResponse internalError(const std::string& message = "内部服务器错误", const std::string& error_code = "INTERNAL_ERROR");

    template<typename T>
    static PaginatedResponse paginated(const std::vector<T>& items, int total, int page, int page_size) {
        return PaginatedResponse(items, total, page, page_size);
    }
};

} // namespace chenglei
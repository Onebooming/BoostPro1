#pragma once
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "../utils/api_response.hpp"

namespace chenglei {
namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

class BaseApiController {
public:
    virtual ~BaseApiController() = default;

    // 处理HTTP请求的统一入口
    virtual http::response<http::string_body> handleRequest(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params) = 0;

protected:
    // HTTP方法处理
    virtual http::response<http::string_body> handleGet(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params);

    virtual http::response<http::string_body> handlePost(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params);

    virtual http::response<http::string_body> handlePut(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params);

    virtual http::response<http::string_body> handleDelete(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params);

    // 工具方法
    http::response<http::string_body> createResponse(
        const ApiResponse& api_response,
        http::status status = http::status::ok);

    http::response<http::string_body> createErrorResponse(
        const ErrorResponse& error_response);

    nlohmann::json parseRequestBody(const http::request<http::string_body>& req);

    std::string getQueryParam(const http::request<http::string_body>& req, const std::string& name);

    template<typename T>
    T getQueryParam(const http::request<http::string_body>& req, const std::string& name, T default_value);

private:
    http::status mapApiStatusToHttpStatus(ApiResponse::Status api_status);
};

// 模板方法实现
template<typename T>
T BaseApiController::getQueryParam(const http::request<http::string_body>& req, const std::string& name, T default_value) {
    std::string value = getQueryParam(req, name);
    if (value.empty()) {
        return default_value;
    }

    if constexpr (std::is_same_v<T, int>) {
        return std::stoi(value);
    } else if constexpr (std::is_same_v<T, long>) {
        return std::stol(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::stod(value);
    } else if constexpr (std::is_same_v<T, float>) {
        return std::stof(value);
    } else {
        return T(value); // 假设有合适的构造函数
    }
}

} // namespace chenglei
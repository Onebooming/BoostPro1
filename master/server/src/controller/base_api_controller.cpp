#include "base_api_controller.hpp"
// #include <boost/url.hpp>  // Temporarily commented out to fix compilation

namespace chenglei {

http::response<http::string_body> BaseApiController::handleRequest(
    const http::request<http::string_body>& req,
    const std::vector<std::string>& path_params) {

    try {
        switch (req.method()) {
            case http::verb::get:
                return handleGet(req, path_params);
            case http::verb::post:
                return handlePost(req, path_params);
            case http::verb::put:
                return handlePut(req, path_params);
            case http::verb::delete_:
                return handleDelete(req, path_params);
            default:
                auto response = createErrorResponse(ResponseBuilder::badRequest("不支持的HTTP方法"));
                response.result(http::status::method_not_allowed);
                return response;
        }
    } catch (const std::exception& e) {
        return createErrorResponse(ResponseBuilder::internalError("服务器内部错误: " + std::string(e.what())));
    }
}

http::response<http::string_body> BaseApiController::handleGet(
    const http::request<http::string_body>& req,
    const std::vector<std::string>& path_params) {

    auto response = createErrorResponse(ResponseBuilder::badRequest("GET方法未实现"));
    response.result(http::status::not_implemented);
    return response;
}

http::response<http::string_body> BaseApiController::handlePost(
    const http::request<http::string_body>& req,
    const std::vector<std::string>& path_params) {

    auto response = createErrorResponse(ResponseBuilder::badRequest("POST方法未实现"));
    response.result(http::status::not_implemented);
    return response;
}

http::response<http::string_body> BaseApiController::handlePut(
    const http::request<http::string_body>& req,
    const std::vector<std::string>& path_params) {

    auto response = createErrorResponse(ResponseBuilder::badRequest("PUT方法未实现"));
    response.result(http::status::not_implemented);
    return response;
}

http::response<http::string_body> BaseApiController::handleDelete(
    const http::request<http::string_body>& req,
    const std::vector<std::string>& path_params) {

    auto response = createErrorResponse(ResponseBuilder::badRequest("DELETE方法未实现"));
    response.result(http::status::not_implemented);
    return response;
}

http::response<http::string_body> BaseApiController::createResponse(
    const ApiResponse& api_response,
    http::status status) {

    http::response<http::string_body> response{status, 11};
    response.set(http::field::server, "BoostPro Educational System");
    response.set(http::field::content_type, "application/json");
    response.set(http::field::access_control_allow_origin, "*");
    response.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
    response.set(http::field::access_control_allow_headers, "Content-Type, Authorization");

    response.body() = api_response.toJson().dump(2);
    response.prepare_payload();

    return response;
}

http::response<http::string_body> BaseApiController::createErrorResponse(
    const ErrorResponse& error_response) {

    http::status http_status = mapApiStatusToHttpStatus(error_response.getStatus());
    return createResponse(error_response, http_status);
}

nlohmann::json BaseApiController::parseRequestBody(const http::request<http::string_body>& req) {
    try {
        std::string body = req.body();
        if (body.empty()) {
            return nlohmann::json::object();
        }
        return nlohmann::json::parse(body);
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("JSON解析错误: " + std::string(e.what()));
    }
}

std::string BaseApiController::getQueryParam(
    const http::request<http::string_body>& req,
    const std::string& name) {

    std::string target = std::string(req.target());
    size_t query_start = target.find('?');
    if (query_start == std::string::npos) {
        return "";
    }

    std::string query_string = target.substr(query_start + 1);

    size_t pos = 0;
    while (pos < query_string.length()) {
        size_t next_pos = query_string.find('&', pos);
        std::string param = (next_pos == std::string::npos) ?
            query_string.substr(pos) : query_string.substr(pos, next_pos - pos);

        size_t eq_pos = param.find('=');
        if (eq_pos != std::string::npos && param.substr(0, eq_pos) == name) {
            return param.substr(eq_pos + 1);
        }

        pos = (next_pos == std::string::npos) ? std::string::npos : next_pos + 1;
    }

    return "";
}

http::status BaseApiController::mapApiStatusToHttpStatus(ApiResponse::Status api_status) {
    switch (api_status) {
        case ApiResponse::Status::SUCCESS:
        case ApiResponse::Status::CREATED:
            return http::status::ok;
        case ApiResponse::Status::BAD_REQUEST:
            return http::status::bad_request;
        case ApiResponse::Status::UNAUTHORIZED:
            return http::status::unauthorized;
        case ApiResponse::Status::FORBIDDEN:
            return http::status::forbidden;
        case ApiResponse::Status::NOT_FOUND:
            return http::status::not_found;
        case ApiResponse::Status::CONFLICT:
            return http::status::conflict;
        case ApiResponse::Status::INTERNAL_ERROR:
        default:
            return http::status::internal_server_error;
    }
}

} // namespace chenglei

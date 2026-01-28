#include "api_response.hpp"

namespace chenglei {

ApiResponse::ApiResponse(Status status, const std::string& message)
    : status_(status), message_(message) {}

nlohmann::json ApiResponse::toJson() const {
    nlohmann::json response;
    response["status"] = static_cast<int>(status_);
    response["message"] = message_;
    return response;
}

SuccessResponse::SuccessResponse(const nlohmann::json& data, const std::string& message)
    : ApiResponse(Status::SUCCESS, message), data_(data) {}

nlohmann::json SuccessResponse::toJson() const {
    nlohmann::json response = ApiResponse::toJson();
    response["success"] = true;
    response["data"] = data_;
    return response;
}

ErrorResponse::ErrorResponse(Status status, const std::string& message, const std::string& error_code)
    : ApiResponse(status, message), error_code_(error_code) {}

nlohmann::json ErrorResponse::toJson() const {
    nlohmann::json response = ApiResponse::toJson();
    response["success"] = false;
    response["error_code"] = error_code_;
    return response;
}

nlohmann::json PaginatedResponse::toJson() const {
    nlohmann::json response = ApiResponse::toJson();
    response["success"] = true;
    response["data"] = items_;
    response["pagination"] = {
        {"total", total_},
        {"page", page_},
        {"page_size", page_size_},
        {"total_pages", (total_ + page_size_ - 1) / page_size_}
    };
    return response;
}

// ResponseBuilder实现
SuccessResponse ResponseBuilder::success(const nlohmann::json& data, const std::string& message) {
    return SuccessResponse(data, message);
}

SuccessResponse ResponseBuilder::success(const std::string& message) {
    return SuccessResponse(nlohmann::json::object(), message);
}

ErrorResponse ResponseBuilder::badRequest(const std::string& message, const std::string& error_code) {
    return ErrorResponse(ApiResponse::Status::BAD_REQUEST, message, error_code);
}

ErrorResponse ResponseBuilder::notFound(const std::string& message, const std::string& error_code) {
    return ErrorResponse(ApiResponse::Status::NOT_FOUND, message, error_code);
}

ErrorResponse ResponseBuilder::conflict(const std::string& message, const std::string& error_code) {
    return ErrorResponse(ApiResponse::Status::CONFLICT, message, error_code);
}

ErrorResponse ResponseBuilder::internalError(const std::string& message, const std::string& error_code) {
    return ErrorResponse(ApiResponse::Status::INTERNAL_ERROR, message, error_code);
}

} // namespace chenglei
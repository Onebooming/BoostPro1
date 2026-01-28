/*
 * @FileName: classroom_controller.hpp
 * @Description: 教室控制器
 */
#ifndef CLASSROOM_CONTROLLER_HPP
#define CLASSROOM_CONTROLLER_HPP

#include "base_controller.hpp"
#include "dao/classroom_dao.hpp"
#include "utils/snowflake.hpp"
#include "../../../public/json.hpp"
#include <boost/beast/http.hpp>
#include <string>
#include <memory>

namespace http = boost::beast::http;
using json = nlohmann::json;

namespace chenglei {

class ClassroomController : public BaseController {
private:
    ClassroomDao& classroomDao = ClassroomDao::getInstance();

public:
    ClassroomController() {
        // 懒加载表创建
    }

    int controller_process(http::request<http::string_body>& request, http::response<http::string_body>& response) override {
        // 确保表存在（懒加载）
        static bool tableCreated = false;
        if (!tableCreated) {
            classroomDao.createTable();
            tableCreated = true;
        }

        std::string url = request.target().to_string();
        std::string action = getQueryParamFromUrl(url, "action");

        if (action.empty() && request.method() == http::verb::get) {
            action = "list_classrooms";
        }

        try {
            if (action == "list_classrooms") {
                return listClassrooms(response);
            } else if (action == "query_by_id") {
                return queryById(request, response);
            } else if (action == "add_classroom") {
                return addClassroom(request, response);
            } else if (action == "update_classroom") {
                return updateClassroom(request, response);
            } else if (action == "delete_classroom") {
                return deleteClassroom(request, response);
            } else if (action == "list_by_building") {
                return listByBuilding(request, response);
            } else if (action == "list_by_type") {
                return listByType(request, response);
            } else if (action == "list_by_status") {
                return listByStatus(request, response);
            } else {
                return errorResponse(response, "Unknown action: " + action, 400);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

private:
    int listClassrooms(http::response<http::string_body>& response) {
        auto classrooms = classroomDao.selectAllClassrooms();

        json jsonArray = json::array();
        for (const auto& classroom : classrooms) {
            jsonArray.push_back(classroomToJson(classroom));
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

        auto classroom = classroomDao.selectClassroomById(id);
        if (!classroom) {
            return errorResponse(response, "Classroom not found", 404);
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = classroomToJson(*classroom);

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int addClassroom(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        try {
            json data = json::parse(request.body());

            // 验证必填字段
            if (!data.contains("room_number") || !data.contains("building")) {
                return errorResponse(response, "Missing required fields: room_number and building", 400);
            }

            Classroom classroom;
            classroom.id = SnowflakeIDGenerator::instance().next_id();
            classroom.room_number = data["room_number"];
            classroom.building = data["building"];
            classroom.floor = data.value("floor", 1);
            classroom.capacity = data.value("capacity", 40);
            classroom.room_type = data.value("room_type", "普通教室");
            classroom.has_projector = data.value("has_projector", false);
            classroom.has_computer = data.value("has_computer", false);
            classroom.has_audio = data.value("has_audio", false);
            classroom.status = data.value("status", "可用");
            classroom.remark = data.value("remark", "");

            if (classroomDao.insertClassroom(classroom)) {
                json jsonResponse;
                jsonResponse["code"] = 200;
                jsonResponse["message"] = "Classroom added successfully";
                jsonResponse["data"] = classroomToJson(classroom);
                return buildJsonResponse(response, jsonResponse, 200);
            } else {
                return errorResponse(response, "Failed to add classroom", 500);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

    int updateClassroom(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        try {
            json data = json::parse(request.body());

            if (!data.contains("id")) {
                return errorResponse(response, "Missing id field", 400);
            }

            auto existingClassroom = classroomDao.selectClassroomById(data["id"]);
            if (!existingClassroom) {
                return errorResponse(response, "Classroom not found", 404);
            }

            // 更新字段
            if (data.contains("room_number")) existingClassroom->room_number = data["room_number"];
            if (data.contains("building")) existingClassroom->building = data["building"];
            if (data.contains("floor")) existingClassroom->floor = data["floor"];
            if (data.contains("capacity")) existingClassroom->capacity = data["capacity"];
            if (data.contains("room_type")) existingClassroom->room_type = data["room_type"];
            if (data.contains("has_projector")) existingClassroom->has_projector = data["has_projector"];
            if (data.contains("has_computer")) existingClassroom->has_computer = data["has_computer"];
            if (data.contains("has_audio")) existingClassroom->has_audio = data["has_audio"];
            if (data.contains("status")) existingClassroom->status = data["status"];
            if (data.contains("remark")) existingClassroom->remark = data["remark"];

            if (classroomDao.updateClassroom(*existingClassroom)) {
                json jsonResponse;
                jsonResponse["code"] = 200;
                jsonResponse["message"] = "Classroom updated successfully";
                jsonResponse["data"] = classroomToJson(*existingClassroom);
                return buildJsonResponse(response, jsonResponse, 200);
            } else {
                return errorResponse(response, "Failed to update classroom", 500);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

    int deleteClassroom(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        try {
            json data = json::parse(request.body());

            if (!data.contains("id")) {
                return errorResponse(response, "Missing id field", 400);
            }

            if (classroomDao.deleteClassroom(data["id"])) {
                json jsonResponse;
                jsonResponse["code"] = 200;
                jsonResponse["message"] = "Classroom deleted successfully";
                return buildJsonResponse(response, jsonResponse, 200);
            } else {
                return errorResponse(response, "Failed to delete classroom", 500);
            }
        } catch (const std::exception& e) {
            return errorResponse(response, std::string("Error: ") + e.what(), 500);
        }
    }

    int listByBuilding(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        std::string url = request.target().to_string();
        std::string building = getQueryParamFromUrl(url, "building");

        if (building.empty()) {
            return errorResponse(response, "Missing building parameter", 400);
        }

        auto classrooms = classroomDao.selectClassroomsByBuilding(building);

        json jsonArray = json::array();
        for (const auto& classroom : classrooms) {
            jsonArray.push_back(classroomToJson(classroom));
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = jsonArray;

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int listByType(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        std::string url = request.target().to_string();
        std::string type = getQueryParamFromUrl(url, "type");

        if (type.empty()) {
            return errorResponse(response, "Missing type parameter", 400);
        }

        auto classrooms = classroomDao.selectClassroomsByType(type);

        json jsonArray = json::array();
        for (const auto& classroom : classrooms) {
            jsonArray.push_back(classroomToJson(classroom));
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = jsonArray;

        return buildJsonResponse(response, jsonResponse, 200);
    }

    int listByStatus(const http::request<http::string_body>& request, http::response<http::string_body>& response) {
        std::string url = request.target().to_string();
        std::string status = getQueryParamFromUrl(url, "status");

        if (status.empty()) {
            return errorResponse(response, "Missing status parameter", 400);
        }

        auto classrooms = classroomDao.selectClassroomsByStatus(status);

        json jsonArray = json::array();
        for (const auto& classroom : classrooms) {
            jsonArray.push_back(classroomToJson(classroom));
        }

        json jsonResponse;
        jsonResponse["code"] = 200;
        jsonResponse["message"] = "success";
        jsonResponse["data"] = jsonArray;

        return buildJsonResponse(response, jsonResponse, 200);
    }

    json classroomToJson(const Classroom& classroom) {
        json j;
        j["id"] = classroom.id;
        j["room_number"] = classroom.room_number;
        j["building"] = classroom.building;
        j["floor"] = classroom.floor;
        j["capacity"] = classroom.capacity;
        j["room_type"] = classroom.room_type;
        j["has_projector"] = classroom.has_projector;
        j["has_computer"] = classroom.has_computer;
        j["has_audio"] = classroom.has_audio;
        j["status"] = classroom.status;
        j["remark"] = classroom.remark;
        j["created_at"] = classroom.created_at;
        j["updated_at"] = classroom.updated_at;
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

#endif // CLASSROOM_CONTROLLER_HPP

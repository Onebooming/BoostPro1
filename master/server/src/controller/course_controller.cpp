#include "course_controller.hpp"
#include "../dao/course_dao.hpp"
#include "../utils/snowflake.hpp"
#include "../../../public/json.hpp"
#include <iostream>

using json = nlohmann::json;

namespace chenglei {

void CourseController::courseToJson(const CourseInfo& course, nlohmann::json& courseJson) const {
    courseJson = {
        {"course_id", course.getCourseId()},
        {"name", course.getCourseName()},
        {"teacher_id", course.getTeacherId()}
    };

    // 处理课程安排
    json schedulesArray = json::array();
    for (const auto& schedule : course.getSchedules()) {
        json scheduleJson = {
            {"location", schedule.location},
            {"start_time", DateTimeUtil::timePointToString(schedule.start_time)},
            {"end_time", DateTimeUtil::timePointToString(schedule.end_time)},
            {"date", schedule.getDate()},
            {"time_range", schedule.getTimeRange()}
        };
        schedulesArray.push_back(scheduleJson);
    }
    courseJson["schedules"] = schedulesArray;
}

CourseInfo CourseController::jsonToCourse(const nlohmann::json& j) const {
    std::string name = j.value("name", "");
    std::string course_id = j.value("course_id", "");
    std::string teacher_id = j.value("teacher_id", "");

    CourseInfo course(name, course_id, teacher_id);

    // 处理课程安排
    if (j.contains("schedules") && j["schedules"].is_array()) {
        for (const auto& scheduleJson : j["schedules"]) {
            std::string location = scheduleJson.value("location", "");
            std::string start_time = scheduleJson.value("start_time", "");
            std::string end_time = scheduleJson.value("end_time", "");

            try {
                course.addSchedule(CourseSchedule(location, start_time, end_time));
            } catch (const std::exception& e) {
                std::cerr << "Invalid schedule format: " << e.what() << std::endl;
            }
        }
    }

    return course;
}

std::string CourseController::query_course(const std::string& id, const std::string& name) {
    json result;
    CourseDao& courseDao = CourseDao::getInstance();

    if (!id.empty()) {
        // 按ID查询
        auto course = courseDao.selectCourseById(id);
        if (course) {
            result = course->toJson();
        } else {
            result = {{"error", "Course not found"}};
        }
    } else if (!name.empty()) {
        // 按名称查询 - 遍历所有课程过滤
        auto allCourses = courseDao.selectAllCourses();
        json arr = json::array();
        for (const auto& course : allCourses) {
            if (course.course_name.find(name) != std::string::npos) {
                arr.push_back(course.toJson());
            }
        }
        result = {{"code", 200}, {"message", "success"}, {"data", arr}};
    } else {
        // 查询所有课程
        auto allCourses = courseDao.selectAllCourses();
        json arr = json::array();
        for (const auto& course : allCourses) {
            arr.push_back(course.toJson());
        }
        result = {{"code", 200}, {"message", "success"}, {"data", arr}};
    }

    return result.dump();
}

std::string CourseController::add_course(const std::string& body) {
    json req = json::parse(body, nullptr, false);
    if (req.is_discarded()) {
        return R"({"error":"invalid json"})";
    }

    try {
        CourseDetailed course = CourseDetailed::fromJson(req);

        // 如果没有提供ID，生成雪花ID
        if (course.id.empty()) {
            course.id = chenglei::SnowflakeIDGenerator::instance().next_id();
        }

        CourseDao& courseDao = CourseDao::getInstance();

        if (courseDao.insertCourse(course)) {
            return R"({"result":"course added"})";
        } else {
            return R"({"error":"failed to add course"})";
        }
    } catch (const std::exception& e) {
        return json{{"error", e.what()}}.dump();
    }
}

std::string CourseController::update_course(const std::string& body) {
    json req = json::parse(body, nullptr, false);
    if (req.is_discarded()) {
        return R"({"error":"invalid json"})";
    }

    try {
        CourseDetailed course = CourseDetailed::fromJson(req);
        CourseDao& courseDao = CourseDao::getInstance();

        if (courseDao.updateCourse(course)) {
            return R"({"result":"course updated"})";
        } else {
            return R"({"error":"failed to update course"})";
        }
    } catch (const std::exception& e) {
        return json{{"error", e.what()}}.dump();
    }
}

std::string CourseController::delete_course(const std::string& id) {
    if (id.empty()) {
        return R"({"error":"missing course id"})";
    }

    CourseDao& courseDao = CourseDao::getInstance();
    if (courseDao.deleteCourseById(id)) {
        return R"({"result":"course deleted"})";
    } else {
        return R"({"error":"course not found or delete failed"})";
    }
}

int CourseController::controller_process(http::request<http::string_body> &request,
                                        http::response<http::string_body> &response) {
    std::string result_json;
    std::string method = request.method_string().to_string();

    try {
        if (request.method() == http::verb::get) {
            // 解析查询参数
            auto target = request.target().to_string();
            auto pos = target.find('?');
            std::string id, name;

            if (pos != std::string::npos) {
                std::string query = target.substr(pos + 1);
                std::istringstream iss(query);
                std::string kv;
                while (std::getline(iss, kv, '&')) {
                    auto eq = kv.find('=');
                    if (eq != std::string::npos) {
                        auto key = kv.substr(0, eq);
                        auto value = kv.substr(eq + 1);
                        if (key == "id") id = value;
                        if (key == "name") name = value;
                    }
                }
            }

            result_json = query_course(id, name);
        }
        else if (request.method() == http::verb::post) {
            result_json = add_course(request.body());
        }
        else if (request.method() == http::verb::put) {
            result_json = update_course(request.body());
        }
        else if (request.method() == http::verb::delete_) {
            // 从URL路径中提取ID /courses?id=123
            auto target = request.target().to_string();
            auto pos = target.find('?');
            std::string id;

            if (pos != std::string::npos) {
                std::string query = target.substr(pos + 1);
                std::istringstream iss(query);
                std::string kv;
                while (std::getline(iss, kv, '&')) {
                    auto eq = kv.find('=');
                    if (eq != std::string::npos) {
                        auto key = kv.substr(0, eq);
                        auto value = kv.substr(eq + 1);
                        if (key == "id") id = value;
                    }
                }
            }

            result_json = delete_course(id);
        }
        else {
            response.result(http::status::bad_request);
            response.body() = R"({"error":"unsupported method"})";
            response.set(http::field::content_type, "application/json");
            response.content_length(response.body().size());
            return 0;
        }

        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, "Boost.Beast");
        response.set(http::field::content_type, "application/json");
        response.body() = result_json;
        response.content_length(response.body().size());
        response.keep_alive(request.keep_alive());

    } catch (const std::exception& e) {
        response.result(http::status::internal_server_error);
        response.body() = json{{"error", e.what()}}.dump();
        response.set(http::field::content_type, "application/json");
        response.content_length(response.body().size());
    }

    return 0;
}

} // namespace chenglei

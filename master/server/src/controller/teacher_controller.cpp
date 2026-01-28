#include "teacher_controller.hpp"
#include "../dao/teacher_dao.hpp"
#include "../../../public/json.hpp"
#include <iostream>

using json = nlohmann::json;

namespace chenglei {

void TeacherController::teacherToJson(const TeacherBaseInfo& teacher, nlohmann::json& teacherJson) const {
    teacherJson = {
        {"id", teacher.getID()},
        {"name", teacher.getName()},
        {"birth_date", teacher.getBirthDate()},
        {"address", teacher.getAddress()},
        {"gender", teacher.getGender()},
        {"hobby", teacher.getHobby()},
        {"phone_number", teacher.getPhoneNumber()},
        {"father_name", teacher.getFatherName()},
        {"mother_name", teacher.getMotherName()},
        {"department", teacher.getDepartment()},
        {"position", teacher.getPosition()},
        {"office", teacher.getOffice()},
        {"teacher_id", teacher.getTeacherID()}
    };
    
    // 处理所授课程
    if (!teacher.getCoursesTaught().empty()) {
        json coursesArray = json::array();
        for (const auto& course_id : teacher.getCoursesTaught()) {
            coursesArray.push_back(course_id);
        }
        teacherJson["courses_taught"] = coursesArray;
    }
}

TeacherBaseInfo TeacherController::jsonToTeacher(const nlohmann::json& j) const {
    TeacherBaseInfo teacher;
    
    // 基础信息
    if (j.contains("name")) teacher.setName(j["name"]);
    if (j.contains("birth_date")) teacher.setBirthDate(j["birth_date"]);
    if (j.contains("address")) teacher.setAddress(j["address"]);
    if (j.contains("id")) teacher.setID(j["id"]);
    if (j.contains("gender")) teacher.setGender(j["gender"]);
    if (j.contains("hobby")) teacher.setHobby(j["hobby"]);
    if (j.contains("phone_number")) teacher.setPhoneNumber(j["phone_number"]);
    if (j.contains("father_name")) teacher.setFatherName(j["father_name"]);
    if (j.contains("mother_name")) teacher.setMotherName(j["mother_name"]);
    
    // 教师特有信息
    if (j.contains("department")) teacher.setDepartment(j["department"]);
    if (j.contains("position")) teacher.setPosition(j["position"]);
    if (j.contains("office")) teacher.setOffice(j["office"]);
    if (j.contains("teacher_id")) teacher.setTeacherID(j["teacher_id"]);
    
    // 所授课程
    if (j.contains("courses_taught") && j["courses_taught"].is_array()) {
        for (const auto& course_id : j["courses_taught"]) {
            teacher.addCourse(course_id);
        }
    }
    
    return teacher;
}

std::string TeacherController::query_teacher(const std::string& id, const std::string& name) {
    std::cout << "[TeacherController] query_teacher called, id='" << id << "', name='" << name << "'" << std::endl;
    json result;
    TeacherDao& teacherDao = TeacherDao::getInstance();

    if (!id.empty()) {
        std::cout << "[TeacherController] Querying by ID" << std::endl;
        // 按ID查询
        auto teacher = teacherDao.selectTeacherById(id);
        if (teacher) {
            teacherToJson(*teacher, result);
        } else {
            result = {{"error", "Teacher not found"}};
        }
    } else if (!name.empty()) {
        std::cout << "[TeacherController] Querying by name" << std::endl;
        // 按名称查询 - 暂时按部门查询或查询所有后过滤
        auto teachers = teacherDao.selectAllTeachers();
        std::cout << "[TeacherController] Got " << teachers.size() << " teachers from DB" << std::endl;
        json arr = json::array();
        for (const auto& teacher : teachers) {
            if (teacher.getName().find(name) != std::string::npos) {
                json teacherJson;
                teacherToJson(teacher, teacherJson);
                arr.push_back(teacherJson);
            }
        }
        result = {{"code", 200}, {"message", "success"}, {"data", arr}};
    } else {
        std::cout << "[TeacherController] Querying all teachers" << std::endl;
        // 查询所有教师
        std::cout << "[TeacherController] Before selectAllTeachers()" << std::endl;
        auto allTeachers = teacherDao.selectAllTeachers();
        std::cout << "[TeacherController] After selectAllTeachers(), got " << allTeachers.size() << " teachers" << std::endl;
        json arr = json::array();
        int count = 0;
        for (const auto& teacher : allTeachers) {
            std::cout << "[TeacherController] Processing teacher " << ++count << std::endl;
            json teacherJson;
            teacherToJson(teacher, teacherJson);
            arr.push_back(teacherJson);
        }
        std::cout << "[TeacherController] Constructing result JSON" << std::endl;
        result = {{"code", 200}, {"message", "success"}, {"data", arr}};
    }

    std::cout << "[TeacherController] Returning result" << std::endl;
    return result.dump();
}

std::string TeacherController::add_teacher(const std::string& body) {
    json req = json::parse(body, nullptr, false);
    if (req.is_discarded()) {
        return R"({"error":"invalid json"})";
    }

    try {
        TeacherBaseInfo teacher = jsonToTeacher(req);
        TeacherDao& teacherDao = TeacherDao::getInstance();

        if (teacherDao.insertTeacher(teacher)) {
            return R"({"result":"teacher added"})";
        } else {
            return R"({"error":"failed to add teacher", "detail":"database insert failed - check server logs for MySQL error"})";
        }
    } catch (const std::exception& e) {
        return json{{"error", e.what()}}.dump();
    }
}

std::string TeacherController::update_teacher(const std::string& body) {
    json req = json::parse(body, nullptr, false);
    if (req.is_discarded()) {
        return R"({"error":"invalid json"})";
    }
    
    try {
        TeacherBaseInfo teacher = jsonToTeacher(req);
        TeacherDao& teacherDao = TeacherDao::getInstance();

        if (teacherDao.updateTeacher(teacher)) {
            return R"({"result":"teacher updated"})";
        } else {
            return R"({"error":"failed to update teacher"})";
        }
    } catch (const std::exception& e) {
        return json{{"error", e.what()}}.dump();
    }
}

std::string TeacherController::delete_teacher(const std::string& id) {
    if (id.empty()) {
        return R"({"error":"missing teacher id"})";
    }
    
    TeacherDao& teacherDao = TeacherDao::getInstance();
    if (teacherDao.deleteTeacherById(id)) {
        return R"({"result":"teacher deleted"})";
    } else {
        return R"({"error":"teacher not found or delete failed"})";
    }
}

int TeacherController::controller_process(http::request<http::string_body> &request,
                                         http::response<http::string_body> &response) {
    std::cout << "[TeacherController] controller_process entered" << std::endl;
    std::string result_json;
    std::string method = request.method_string().to_string();
    std::cout << "[TeacherController] HTTP method: " << method << std::endl;

    try {
        if (request.method() == http::verb::get) {
            std::cout << "[TeacherController] Processing GET request" << std::endl;
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

            std::cout << "[TeacherController] About to call query_teacher, id='" << id << "', name='" << name << "'" << std::endl;
            result_json = query_teacher(id, name);
            std::cout << "[TeacherController] query_teacher returned, result length=" << result_json.length() << std::endl;
        } 
        else if (request.method() == http::verb::post) {
            result_json = add_teacher(request.body());
        } 
        else if (request.method() == http::verb::put) {
            result_json = update_teacher(request.body());
        } 
        else if (request.method() == http::verb::delete_) {
            // 从URL路径中提取ID /teachers?id=123
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
            
            result_json = delete_teacher(id);
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

        std::cout << "[TeacherController] Response prepared successfully, returning" << std::endl;

    } catch (const std::exception& e) {
        std::cout << "[TeacherController] EXCEPTION: " << e.what() << std::endl;
        response.result(http::status::internal_server_error);
        response.body() = json{{"error", e.what()}}.dump();
        response.set(http::field::content_type, "application/json");
        response.content_length(response.body().size());
    }

    std::cout << "[TeacherController] controller_process exiting" << std::endl;
    return 0;
}

} // namespace chenglei
/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-07 22:34:05
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-22 22:41:27
 * @FilePath: /BoostPro1/master/server/src/controller/student_controller.cpp
 * @Description: student controller 类的文件
 */

#include <iostream>
#include "student_controller.hpp"
#include "../dao/student_dao.hpp"
#include "../../../public/json.hpp"

using json = nlohmann::json;

namespace chenglei {

void transStuObj2Json(const StudentBaseInfo &stuObj, json &stuJson)
{
    stuJson = {
        {"id", stuObj.getID()},
        {"name", stuObj.getName()},
        {"birth_date", stuObj.getBirthDate()},
        {"address", stuObj.getAddress()},
        {"gender", stuObj.getGender()},
        {"hobby", stuObj.getHobby()},
        {"phone_number", stuObj.getPhoneNumber()},
        {"father_name", stuObj.getFatherName()},
        {"mother_name", stuObj.getMotherName()},
        {"grade", stuObj.getGrade()},
        {"class_name", stuObj.getClassName()},
        {"major", stuObj.getMajor()},
        {"student_id", stuObj.getStudentID()}
    };
}

std::string StudentController::query_student(const std::string& id, const std::string& name) {
    // 实际业务可查数据库/内存等
    json result;
    if (!id.empty()) {
        result = { {"id", id}, {"name", "example_name"} }; // 示例
    } else if (!name.empty()) {
        result = { {"id", "1"}, {"name", name} }; //从数据库查
        /* 从mysql数据库查询真正的数据 */   
    } else {
        /* 获取所有的学生数据 */
        json arr = json::array();
        StudentDao stuDao;
        std::vector<StudentBaseInfo> studentList = stuDao.selectAllStudents();
        for(auto& stu : studentList)
        {
            json stuJson;
            transStuObj2Json(stu, stuJson);
            arr.push_back(stuJson);
        }
        result = arr;
    }
    return result.dump();
}


StudentBaseInfo json_to_SB_obj(const json &req)
{
    StudentBaseInfo stu;
    
    if(req.contains("name"))
        stu.setName(req.value("name", ""));
    if (req.contains("birth_date")) stu.setBirthDate(req.value("birth_date", ""));
    if (req.contains("address")) stu.setAddress(req.value("address", ""));
    if (req.contains("id")) stu.setID(req.value("id", "")); // 可选
    if (req.contains("gender")) stu.setGender(req.value("gender", ""));
    if (req.contains("hobby")) stu.setHobby(req.value("hobby", ""));
    if (req.contains("phone_number")) stu.setPhoneNumber(req.value("phone_number", ""));
    if (req.contains("father_name")) stu.setFatherName(req.value("father_name", ""));
    if (req.contains("mother_name")) stu.setMotherName(req.value("mother_name", ""));

    // 学生扩展信息
    if (req.contains("grade")) stu.setGrade(req.value("grade", ""));
    if (req.contains("class_name")) stu.setClassName(req.value("class_name", ""));
    if (req.contains("major")) stu.setMajor(req.value("major", ""));
    if (req.contains("student_id")) stu.setStudentID(req.value("student_id", ""));

    return stu;
}

std::string StudentController::add_student(const std::string& body) {
    json req = json::parse(body, nullptr, false);
    if (req.is_discarded()) {
        return R"({"error":"invalid json"})";
    }
    // 实际业务可写数据库等
    StudentBaseInfo student = json_to_SB_obj(req);
    StudentDao stuDao;
    stuDao.insertStudent(student);
    return R"({"result":"student added"})";
}

int StudentController::controller_process(http::request<http::string_body> &request, http::response<http::string_body> &response)
{
    std::string result_json;

    if (request.method() == http::verb::get) {
        // 解析参数
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
        std::cout << "get method.\n";
        result_json = query_student(id, name);
    } else if (request.method() == http::verb::post){
        result_json = add_student(request.body());
        std::cout << "post method.\n";
    } else {
        response.result(http::status::bad_request);
        response.body() = "{\"error\":\"unsupported method\"}";
        std::cout << "other method.\n";
        return 0;
    }

    std::cout << "Received request: " << request.body() << std::endl;
    std::cout << "Sending response: " << result_json << std::endl;

    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "Boost.Beast");
    response.set(http::field::content_type, "application/json");
    response.body() = result_json;
    response.content_length(response.body().size());
    response.keep_alive(request.keep_alive());

    return 0;
}
}
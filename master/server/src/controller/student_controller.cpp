/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-07 22:34:05
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-21 22:15:07
 * @FilePath: /BoostPro1/master/server/src/controller/student_controller.cpp
 * @Description: student controller 类的文件
 */

#include "student_controller.hpp"
#include "../dao/student_dao.hpp"
#include "../../../public/json.hpp"

using json = nlohmann::json;

namespace chenglei {

std::string StudentController::query_student(const std::string& id, const std::string& name) {
    // 实际业务可查数据库/内存等
    json result;
    if (!id.empty()) {
        result = { {"id", id}, {"name", "example_name"} }; // 示例
    } else if (!name.empty()) {
        result = { {"id", "1"}, {"name", name} }; //从数据库查
        /* 从mysql数据库查询真正的数据 */
        
    } else {
        result = { {"error", "missing parameter"} };
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

}
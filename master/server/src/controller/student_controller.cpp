/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-07 22:34:05
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-07 22:38:01
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

std::string StudentController::add_student(const std::string& body) {
    json req = json::parse(body, nullptr, false);
    if (req.is_discarded()) {
        return R"({"error":"invalid json"})";
    }
    // 实际业务可写数据库等
    return R"({"result":"student added"})";
}

}
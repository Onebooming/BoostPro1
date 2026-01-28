/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-26 22:19:34
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-26 22:19:43
 * @FilePath: /BoostPro1/master/server/src/controller/teacher_controller.hpp
 * @Description: teacher controller类的头文件
 */
#ifndef _TEACHER_CONTROLLER_HPP_
#define _TEACHER_CONTROLLER_HPP_

#include <string>
#include "base_controller.hpp"
#include "../entity/teacher.hpp"

namespace chenglei {

class TeacherController : public BaseController {
public:
    // 查询教师
    std::string query_teacher(const std::string& id, const std::string& name);
    
    // 新增教师
    std::string add_teacher(const std::string& body);
    
    // 更新教师
    std::string update_teacher(const std::string& body);
    
    // 删除教师
    std::string delete_teacher(const std::string& id);
    
    int controller_process(http::request<http::string_body> &request, 
                          http::response<http::string_body> &response) override;

private:
    // 教师对象转JSON
    void teacherToJson(const TeacherBaseInfo& teacher, nlohmann::json& teacherJson) const;
    
    // JSON转教师对象
    TeacherBaseInfo jsonToTeacher(const nlohmann::json& j) const;
};

} // namespace chenglei

#endif //_TEACHER_CONTROLLER_HPP_
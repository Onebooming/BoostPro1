/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-26 22:18:34
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-26 22:18:42
 * @FilePath: /BoostPro1/master/server/src/controller/course_controller.hpp
 * @Description: course controller类的头文件
 */
#ifndef _COURSE_CONTROLLER_HPP_
#define _COURSE_CONTROLLER_HPP_

#include <string>
#include "base_controller.hpp"
#include "../entity/course.hpp"

namespace chenglei {

class CourseController : public BaseController {
public:
    // 查询课程
    std::string query_course(const std::string& id, const std::string& name);
    
    // 新增课程
    std::string add_course(const std::string& body);
    
    // 更新课程
    std::string update_course(const std::string& body);
    
    // 删除课程
    std::string delete_course(const std::string& id);
    
    int controller_process(http::request<http::string_body> &request, 
                           http::response<http::string_body> &response) override;

private:
    // 课程对象转JSON
    void courseToJson(const CourseInfo& course, nlohmann::json& courseJson) const;
    
    // JSON转课程对象
    CourseInfo jsonToCourse(const nlohmann::json& j) const;
};

} // namespace chenglei

#endif //_COURSE_CONTROLLER_HPP_
/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-07 22:32:27
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-22 22:51:29
 * @FilePath: /BoostPro1/master/server/src/controller/student_controller.hpp
 * @Description: student controller 类的头文件
 */
#ifndef _STUDENT_CONTROLLER_HPP_
#define _STUDENT_CONTROLLER_HPP_

#include <string>
#include "base_controller.hpp"

namespace chenglei {

class StudentController : public BaseController {
public:
    // 查询学生
    std::string query_student(const std::string& id, const std::string& name);
    // 新增学生
    std::string add_student(const std::string& body);

    int controller_process(http::request<http::string_body> &request, http::response<http::string_body> &response) override;
};
}

#endif //_STUDENT_CONTROLLER_HPP_
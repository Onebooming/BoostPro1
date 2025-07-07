#ifndef _STUDENT_CONTROLLER_HPP_
#define _STUDENT_CONTROLLER_HPP_

#include <string>

namespace chenglei {

class StudentController {
public:
    // 查询学生
    std::string query_student(const std::string& id, const std::string& name);
    // 新增学生
    std::string add_student(const std::string& body);
};

}

#endif //_STUDENT_CONTROLLER_HPP_
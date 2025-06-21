/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-21 17:13:36
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-21 17:14:29
 * @FilePath: /BoostPro1/master/server/src/dao/student_dao.hpp
 * @Description: student dao操作
 */
#pragma once
#include "../entity/student.hpp"
#include <mysql/mysql.h>
#include <string>
#include <vector>

namespace chenglei {

class StudentDAO {
public:
    StudentDAO();
    ~StudentDAO();

    bool addStudent(const StudentBaseInfo& student);
    bool deleteStudent(const std::string& student_id);
    bool updateStudent(const StudentBaseInfo& student);
    StudentBaseInfo getStudent(const std::string& student_id);

private:
    MYSQL* connection_;
    bool connectToDatabase();
    void closeConnection();
};

} // namespace chenglei
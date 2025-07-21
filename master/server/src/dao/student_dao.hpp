/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-21
 * @Description: StudentBaseInfo数据库操作DAO
 */
#pragma once
#include "../entity/student.hpp"
#include <vector>
#include <string>
#include <optional>

namespace chenglei {

class StudentDao {
public:
    // 增
    bool insertStudent(const StudentBaseInfo& student);

    // 删
    bool deleteStudentById(const std::string& id);

    // 改
    bool updateStudent(const StudentBaseInfo& student);

    // 查单个
    std::optional<StudentBaseInfo> selectStudentById(const std::string& id);

    // 查全部
    std::vector<StudentBaseInfo> selectAllStudents();

    // 按学号查
    std::optional<StudentBaseInfo> selectStudentByStudentId(const std::string& student_id);

    // 按手机号查
    std::optional<StudentBaseInfo> selectStudentByPhone(const std::string& phone_number);
};

} // namespace chenglei
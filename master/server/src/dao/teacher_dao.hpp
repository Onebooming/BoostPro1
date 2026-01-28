#pragma once
#include "../entity/teacher.hpp"
#include <vector>
#include <optional>
#include <memory>
#include <mysql/mysql.h>

namespace chenglei {

class TeacherDao {
public:
    static TeacherDao& getInstance() {
        static TeacherDao instance;
        return instance;
    }

    // CRUD 操作
    bool insertTeacher(const TeacherBaseInfo& teacher);
    bool deleteTeacherById(const std::string& id);
    bool updateTeacher(const TeacherBaseInfo& teacher);

    // 查询操作
    std::optional<TeacherBaseInfo> selectTeacherById(const std::string& id);
    std::optional<TeacherBaseInfo> selectTeacherByTeacherId(const std::string& teacher_id);
    std::vector<TeacherBaseInfo> selectAllTeachers();
    std::vector<TeacherBaseInfo> selectTeachersByDepartment(const std::string& department);

    // 课程相关查询
    std::vector<TeacherBaseInfo> selectTeachersWithCourses();

private:
    TeacherDao() = default;
    TeacherDao(const TeacherDao&) = delete;
    TeacherDao& operator=(const TeacherDao&) = delete;

    // 工具函数
    TeacherBaseInfo rowToTeacher(const MYSQL_ROW row);
    std::vector<std::string> getCoursesByTeacherId(const std::string& teacher_id);
};

} // namespace chenglei
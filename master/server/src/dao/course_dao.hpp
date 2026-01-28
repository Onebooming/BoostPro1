#pragma once
#include "../entity/course_detailed.hpp"
#include "../entity/course.hpp"
#include <vector>
#include <optional>
#include <memory>
#include <mysql/mysql.h>

namespace chenglei {

class CourseDao {
public:
    static CourseDao& getInstance() {
        static CourseDao instance;
        return instance;
    }

    // CRUD 操作
    bool insertCourse(const CourseDetailed& course);
    bool deleteCourseById(const std::string& id);
    bool updateCourse(const CourseDetailed& course);

    // 基本查询操作
    std::optional<CourseDetailed> selectCourseById(const std::string& id);
    std::optional<CourseDetailed> selectCourseByCourseId(const std::string& course_id);
    std::vector<CourseDetailed> selectAllCourses();
    std::vector<CourseDetailed> selectCoursesByTeacher(const std::string& teacher_id);
    std::vector<CourseDetailed> selectCoursesByType(const std::string& course_type);
    std::vector<CourseDetailed> selectCoursesBySemester(const std::string& semester);

    // 复杂查询操作
    std::vector<CourseDetailed> searchCourses(const CourseQueryParams& params);
    int countCourses(const CourseQueryParams& params);

    // 课程统计
    CourseStatistics getCourseStatistics();

    // 课程时间安排
    bool insertCourseSchedule(const std::string& course_id, const CourseDetailed::CourseSchedule& schedule);
    bool updateCourseSchedule(const std::string& course_id, const CourseDetailed::CourseSchedule& schedule);
    bool deleteCourseSchedules(const std::string& course_id);
    std::vector<CourseDetailed::CourseSchedule> getCourseSchedules(const std::string& course_id);

    // 检查课程时间冲突
    bool hasTimeConflict(const std::string& teacher_id, const CourseDetailed::CourseSchedule& schedule,
                        const std::string& exclude_course_id = "");

private:
    CourseDao() = default;
    CourseDao(const CourseDao&) = delete;
    CourseDao& operator=(const CourseDao&) = delete;

    // 工具函数
    CourseDetailed rowToCourse(MYSQL* conn, const MYSQL_ROW row, bool with_schedules = false, bool with_teacher_info = false);
    std::vector<CourseDetailed::CourseSchedule> getSchedulesForCourse(MYSQL* conn, const std::string& course_id);
    std::string getTeacherName(MYSQL* conn, const std::string& teacher_id);
    std::string buildWhereClause(const CourseQueryParams& params);
    std::string buildOrderClause(const CourseQueryParams& params);
};

} // namespace chenglei
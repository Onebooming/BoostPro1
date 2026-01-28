#pragma once
#include "course.hpp"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

namespace chenglei {

// 完整的课程信息类（包含数据库字段）
struct CourseDetailed {
    std::string id;                    // 雪花ID
    std::string course_id;            // 课程编号
    std::string course_name;          // 课程名称
    std::string teacher_id;           // 授课教师ID
    std::string teacher_name;         // 教师姓名（用于显示）
    double credits;                   // 学分
    int max_students;                 // 最大选课人数
    int current_students;             // 当前选课人数
    std::string course_type;          // 课程类型（必修、选修、专业选修）
    std::string semester;             // 学期
    std::string description;          // 课程描述
    std::string created_at;           // 创建时间
    std::string updated_at;           // 更新时间

    // 课程时间安排
    struct CourseSchedule {
        std::string day_of_week;      // 星期几
        std::string start_time;       // 开始时间
        std::string end_time;         // 结束时间
        std::string location;         // 上课地点
        std::string week_range;       // 周次范围
    };
    std::vector<CourseSchedule> schedules;

    // JSON序列化
    nlohmann::json toJson() const;
    static CourseDetailed fromJson(const nlohmann::json& json);
};

// 课程查询条件
struct CourseQueryParams {
    std::string teacher_id;           // 教师ID
    std::string course_type;          // 课程类型
    std::string semester;             // 学期
    std::string keyword;              // 关键词（课程名称或编号）
    int page = 1;                     // 页码
    int page_size = 10;               // 每页数量
    bool with_schedules = false;      // 是否包含时间安排
    bool with_teacher_info = false;   // 是否包含教师信息
};

// 课程统计信息
struct CourseStatistics {
    int total_courses = 0;            // 总课程数
    int total_students = 0;           // 总选课学生数
    double average_students_per_course = 0.0; // 平均每门课的选课人数
    std::unordered_map<std::string, int> courses_by_type; // 按类型统计
    std::unordered_map<std::string, int> courses_by_semester; // 按学期统计
};

} // namespace chenglei
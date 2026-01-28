#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "../utils/snowflake.hpp"
#include "../utils/date_time_utils.hpp"

namespace chenglei {

struct CourseSchedule {

    std::chrono::system_clock::time_point start_time;
    std::chrono::system_clock::time_point end_time;
    std::string location;

    CourseSchedule(const std::string& start, const std::string& end, const std::string& location);

    CourseSchedule(const std::chrono::system_clock::time_point& start,
                    const std::chrono::system_clock::time_point& end,
                    const std::string& loc);

    std::string getDate() const;
    std::string getClassTime() const;
    std::string getTimeRange() const;
};

class CourseInfo {
public:
    // 核心构造函数
    CourseInfo(const std::string& name, const std::string& id, const std::string& teacher_id);
    
    // 特殊成员函数
    CourseInfo() = default;
    CourseInfo(const CourseInfo& other);
    CourseInfo(CourseInfo&& other) noexcept;
    CourseInfo& operator=(const CourseInfo& other);
    CourseInfo& operator=(CourseInfo&& other) noexcept;
    ~CourseInfo() = default;

    // 原有接口
    void addSchedule(const CourseSchedule& schedule);
    const std::string& getCourseName() const { return course_name_; }
    const std::string& getCourseId() const { return course_id_; }
    const std::string& getTeacherId() const { return teacher_id_; }
    const std::vector<CourseSchedule>& getSchedules() const { return schedules_; }
    void addSchedule(const std::string& date, const std::string& time_range, const std::string& location);
    void clearSchedules() { schedules_.clear(); }
    bool hasScheduleConflict() const;
    bool hasConflictWith(const CourseInfo& other) const;

private:
    std::string course_name_;
    std::string course_id_;
    std::string teacher_id_;
    std::vector<CourseSchedule> schedules_;
};

} // namespace chenglei
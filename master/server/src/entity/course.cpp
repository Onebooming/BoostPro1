#include "course.hpp"
#include <iostream>
#include <algorithm>

namespace chenglei {

// CourseSchedule 实现
CourseSchedule::CourseSchedule(const std::string& start, const std::string& end, const std::string& location)
    : start_time(DateTimeUtil::stringToTimePoint(start)),
      end_time(DateTimeUtil::stringToTimePoint(end)),
      location(location) 
{
    if (start_time >= end_time) {
        throw std::invalid_argument("End time must be after start time");
    }
}

CourseSchedule::CourseSchedule(const std::chrono::system_clock::time_point& start,
                                const std::chrono::system_clock::time_point& end,
                                const std::string& loc)
    : start_time(start), end_time(end), location(loc) 
{
    if (start_time >= end_time) {
        throw std::invalid_argument("End time must be after start time");
    }
}

std::string CourseSchedule::getDate() const {
    return DateTimeUtil::timePointToDateString(start_time);
}

std::string CourseSchedule::getClassTime() const {
    return DateTimeUtil::timePointToDateString(start_time) + " " + getTimeRange();
}

std::string CourseSchedule::getTimeRange() const {
    return DateTimeUtil::timePointToTimeString(start_time) + "-" + 
            DateTimeUtil::timePointToTimeString(end_time);
}

// CourseInfo 实现
CourseInfo::CourseInfo(const std::string& name, 
                        const std::string& id, 
                        const std::string& teacher_id)
    : course_name_(name), course_id_(id), teacher_id_(teacher_id) {}

// 拷贝构造函数
CourseInfo::CourseInfo(const CourseInfo& other)
    : course_name_(other.course_name_),
        course_id_(other.course_id_),
        teacher_id_(other.teacher_id_),
        schedules_(other.schedules_) {}

// 移动构造函数
CourseInfo::CourseInfo(CourseInfo&& other) noexcept
    : course_name_(std::move(other.course_name_)),
        course_id_(std::move(other.course_id_)),
        teacher_id_(std::move(other.teacher_id_)),
        schedules_(std::move(other.schedules_)) {}

// 拷贝赋值运算符
CourseInfo& CourseInfo::operator=(const CourseInfo& other) {
    if (this != &other) {
        course_name_ = other.course_name_;
        course_id_ = other.course_id_;
        teacher_id_ = other.teacher_id_;
        schedules_ = other.schedules_;
    }
    return *this;
}

// 移动赋值运算符
CourseInfo& CourseInfo::operator=(CourseInfo&& other) noexcept {
    if (this != &other) {
        course_name_ = std::move(other.course_name_);
        course_id_ = std::move(other.course_id_);
        teacher_id_ = std::move(other.teacher_id_);
        schedules_ = std::move(other.schedules_);
    }
    return *this;
}

void CourseInfo::addSchedule(const CourseSchedule& schedule) {
    schedules_.push_back(schedule);
}

void CourseInfo::addSchedule(const std::string& date, 
                                const std::string& time_range, 
                                const std::string& location) {
    auto [start_str, end_str] = DateTimeUtil::parseTimeRange(time_range);
    auto start_time = DateTimeUtil::stringToTimePoint(date + " " + start_str);
    auto end_time = DateTimeUtil::stringToTimePoint(date + " " + end_str);
    schedules_.emplace_back(CourseSchedule(start_time, end_time, location));
}

bool CourseInfo::hasScheduleConflict() const {
    // 优化为 O(n log n) 实现
    std::vector<CourseSchedule> sorted = schedules_;
    std::sort(sorted.begin(), sorted.end(), 
        [](const CourseSchedule& a, const CourseSchedule& b) {
            return a.start_time < b.start_time;
        });

    for (size_t i = 1; i < sorted.size(); ++i) {
        if (sorted[i].start_time < sorted[i-1].end_time) {
            return true;
        }
    }
    return false;
}

bool CourseInfo::hasConflictWith(const CourseInfo& other) const {
    // 使用更简洁的时间重叠判断
    for (const auto& s1 : schedules_) {
        for (const auto& s2 : other.schedules_) {
            if (s1.start_time < s2.end_time && s2.start_time < s1.end_time) {
                return true;
            }
        }
    }
    return false;
}

// 单元测试保持不变
[[maybe_unused]] 
static void unit_test_of() {
    using namespace chenglei;

    // 创建课程信息
    CourseInfo course1("Math", "C001", "T001");
    course1.addSchedule("2023/10/01", "08:00-10:00", "Room 101");
    course1.addSchedule("2023/10/01", "10:00-12:00", "Room 102");

    CourseInfo course2("Physics", "C002", "T002");
    course2.addSchedule("2023/10/01", "09:00-11:00", "Room 103");

    // 添加课程安排 (时间点接口)
    auto start = chenglei::DateTimeUtil::stringToTimePoint("2023/09/03 14:00");
    auto end = chenglei::DateTimeUtil::stringToTimePoint("2023/09/03 16:00");
    course2.addSchedule({start, end, "Building B-202"});
    
    // 检查时间冲突 
    if (course1.hasConflictWith(course2)) {
        std::cout << "Courses have time conflict!" << std::endl;
    } else {
        std::cout << "No time conflict between courses." << std::endl;
    }
}

} // namespace chenglei
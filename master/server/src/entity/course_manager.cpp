#include "course_manager.hpp"
#include <fstream>
#include <algorithm>

namespace chenglei {

// ... 其他成员函数实现保持不变 ...
bool CourseManager::addCourse(const CourseInfo& course) {
    const std::string& id = course.getCourseId();
    
    // 检查课程ID是否已存在
    if (courses_.find(id) != courses_.end()) {
        return false;
    }
    
    // 检查新课程与现有课程冲突
    if (hasConflictWithAll(course)) {
        return false;
    }
    
    // 检查课程内部冲突
    if (course.hasScheduleConflict()) {
        return false;
    }
    
    courses_.emplace(id, course);
    return true;
}

bool CourseManager::addCourse(CourseInfo&& course) {
    const std::string& id = course.getCourseId();
    
    if (courses_.find(id) != courses_.end()) {
        return false;
    }
    
    if (hasConflictWithAll(course)) {
        return false;
    }
    
    if (course.hasScheduleConflict()) {
        return false;
    }
    
    courses_.emplace(id, std::move(course));
    return true;
}

bool CourseManager::removeCourse(const std::string& course_id) {
    auto it = courses_.find(course_id);
    if (it == courses_.end()) {
        return false;
    }
    
    courses_.erase(it);
    return true;
}

CourseInfo* CourseManager::getCourse(const std::string& course_id) {
    auto it = courses_.find(course_id);
    return (it != courses_.end()) ? &it->second : nullptr;
}

const CourseInfo* CourseManager::getCourse(const std::string& course_id) const {
    auto it = courses_.find(course_id);
    return (it != courses_.end()) ? &it->second : nullptr;
}

std::vector<CourseInfo> CourseManager::getAllCourses() const {
    std::vector<CourseInfo> result;
    result.reserve(courses_.size());
    
    for (const auto& pair : courses_) {
        result.push_back(pair.second);
    }
    
    return result;
}

bool CourseManager::updateCourse(const CourseInfo& course) {
    const std::string& id = course.getCourseId();
    auto it = courses_.find(id);
    
    if (it == courses_.end()) {
        return false;
    }
    
    // 创建临时副本用于冲突检查
    CourseInfo temp = it->second;
    it->second = course;
    
    // 检查更新后是否冲突
    if (hasConflictWithAll(course) || course.hasScheduleConflict()) {
        // 恢复原始数据
        it->second = std::move(temp);
        return false;
    }
    
    return true;
}

bool CourseManager::hasCourseConflict(const std::string& course_id) const {
    const CourseInfo* course = getCourse(course_id);
    return course ? course->hasScheduleConflict() : false;
}

bool CourseManager::hasCoursesConflict(const std::string& course_id1, const std::string& course_id2) const {
    const CourseInfo* course1 = getCourse(course_id1);
    const CourseInfo* course2 = getCourse(course_id2);
    
    if (!course1 || !course2) {
        return false;
    }
    
    return course1->hasConflictWith(*course2);
}

bool CourseManager::hasConflictWithAll(const CourseInfo& new_course) const {
    for (const auto& pair : courses_) {
        if (pair.second.hasConflictWith(new_course)) {
            return true;
        }
    }
    return false;
}

std::vector<CourseInfo> CourseManager::getCoursesByTeacher(const std::string& teacher_id) const {
    std::vector<CourseInfo> result;
    
    for (const auto& pair : courses_) {
        if (pair.second.getTeacherId() == teacher_id) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<CourseInfo> CourseManager::getCoursesByTimeRange(
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) const 
{
    std::vector<CourseInfo> result;
    
    for (const auto& pair : courses_) {
        const auto& schedules = pair.second.getSchedules();
        for (const auto& schedule : schedules) {
            // 检查课程安排是否与时间范围有重叠
            if (!(schedule.end_time <= start || schedule.start_time >= end)) {
                result.push_back(pair.second);
                break; // 课程只要有一个安排匹配就包含
            }
        }
    }
    
    return result;
}

// 序列化为 nlohmann::json
json CourseManager::toJson() const {
    json j = json::array(); // 使用数组存储所有课程
    
    for (const auto& pair : courses_) {
        const CourseInfo& course = pair.second;
        json courseJson;
        
        courseJson["course_id"] = course.getCourseId();
        courseJson["name"] = course.getCourseName();
        courseJson["teacher_id"] = course.getTeacherId();
        
        json schedulesArray = json::array();
        for (const auto& schedule : course.getSchedules()) {
            json scheduleJson;
            scheduleJson["location"] = schedule.location;
            scheduleJson["start_time"] = DateTimeUtil::timePointToString(schedule.start_time);
            scheduleJson["end_time"] = DateTimeUtil::timePointToString(schedule.end_time);
            schedulesArray.push_back(scheduleJson);
        }
        
        courseJson["schedules"] = schedulesArray;
        j.push_back(courseJson);
    }
    
    return j;
}

// 从 nlohmann::json 反序列化
bool CourseManager::fromJson(const json& j) {
    if (!j.is_array()) {
        return false;
    }
    
    std::unordered_map<std::string, CourseInfo> newCourses;
    
    for (const auto& courseJson : j) {
        // 验证必需字段
        if (!courseJson.contains("course_id") || 
            !courseJson.contains("name") ||
            !courseJson.contains("teacher_id") ||
            !courseJson.contains("schedules")) 
        {
            return false;
        }
        
        std::string course_id = courseJson["course_id"].get<std::string>();
        std::string name = courseJson["name"].get<std::string>();
        std::string teacher_id = courseJson["teacher_id"].get<std::string>();
        
        CourseInfo course(name, course_id, teacher_id);
        
        // 处理课程安排
        const auto& schedulesJson = courseJson["schedules"];
        if (!schedulesJson.is_array()) {
            return false;
        }
        
        for (const auto& scheduleJson : schedulesJson) {
            if (!scheduleJson.contains("location") ||
                !scheduleJson.contains("start_time") ||
                !scheduleJson.contains("end_time")) 
            {
                return false;
            }
            
            std::string location = scheduleJson["location"].get<std::string>();
            std::string start_str = scheduleJson["start_time"].get<std::string>();
            std::string end_str = scheduleJson["end_time"].get<std::string>();
            
            try {
                course.addSchedule(location, start_str, end_str);
            } catch (const std::exception& e) {
                // 处理无效时间格式
                return false;
            }
        }
        
        // 检查课程ID是否重复
        if (newCourses.find(course_id) != newCourses.end()) {
            return false;
        }
        
        newCourses.emplace(course_id, std::move(course));
    }
    
    // 检查所有课程间冲突
    for (const auto& pair1 : newCourses) {
        // 检查课程内部冲突
        if (pair1.second.hasScheduleConflict()) {
            return false;
        }
        
        // 检查与其他课程冲突
        for (const auto& pair2 : newCourses) {
            if (pair1.first != pair2.first && pair1.second.hasConflictWith(pair2.second)) {
                return false;
            }
        }
    }
    
    // 所有检查通过，替换当前课程数据
    courses_ = std::move(newCourses);
    return true;
}

bool CourseManager::saveToFile(const std::string& filename) const {
    try {
        json j = toJson();
        std::ofstream ofs(filename);
        if (!ofs) return false;
        
        ofs << j.dump(4); // 缩进4个空格的美化输出
        return true;
    } catch (...) {
        return false;
    }
}

bool CourseManager::loadFromFile(const std::string& filename) {
    try {
        std::ifstream ifs(filename);
        if (!ifs) return false;
        
        json j;
        ifs >> j;
        return fromJson(j);
    } catch (...) {
        return false;
    }
}

} // namespace chenglei
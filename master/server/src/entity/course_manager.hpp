/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-26 20:59:57
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-26 21:00:04
 * @FilePath: /BoostPro1/master/server/src/entity/course_manager.hpp
 * @Description: 课程管理类的头文件
 */
#pragma once
#include "course.hpp"
#include <unordered_map>
#include <set>
#include <memory>
#include <fstream>
#include "../../../public/json.hpp" // 使用 nlohmann::json

using json = nlohmann::json; // 命名空间别名

namespace chenglei {

class CourseManager {
public:
    static CourseManager& getInstance() {
        static CourseManager instance;
        return instance;
    }
    
    CourseManager(const CourseManager&) = delete;
    CourseManager& operator=(const CourseManager&) = delete;
    
    bool addCourse(const CourseInfo& course);
    bool addCourse(CourseInfo&& course);
    bool removeCourse(const std::string& course_id);
    
    CourseInfo* getCourse(const std::string& course_id);
    const CourseInfo* getCourse(const std::string& course_id) const;
    
    std::vector<CourseInfo> getAllCourses() const;
    bool updateCourse(const CourseInfo& course);
    
    bool hasCourseConflict(const std::string& course_id) const;
    bool hasCoursesConflict(const std::string& course_id1, const std::string& course_id2) const;
    bool hasConflictWithAll(const CourseInfo& new_course) const;
    
    std::vector<CourseInfo> getCoursesByTeacher(const std::string& teacher_id) const;
    std::vector<CourseInfo> getCoursesByTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    
    // 修改为 nlohmann::json
    json toJson() const;
    bool fromJson(const json& j);
    
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    
private:
    CourseManager() = default;
    ~CourseManager() = default;
    
    std::unordered_map<std::string, CourseInfo> courses_;
};

} // namespace chenglei
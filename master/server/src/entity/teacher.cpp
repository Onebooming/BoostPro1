/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-27
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-26 21:12:03
 * @FilePath: /BoostPro1/master/server/src/entity/teacher.cpp
 * @Description: 教师个人信息类与管理类实现
 */
#include "teacher.hpp"

namespace chenglei {

void TeacherBaseInfo::removeCourse(const std::string& course_id) {
    auto it = std::find(courses_taught_.begin(), courses_taught_.end(), course_id);
    if (it != courses_taught_.end()) {
        courses_taught_.erase(it);
    }
}

bool TeacherMgr::addTeacher(std::unique_ptr<TeacherBaseInfo> teacher) {
    std::lock_guard<std::mutex> lock(mutex_);
    const std::string& id = teacher->getTeacherID();
    
    if (id.empty() || teacher_map_.find(id) != teacher_map_.end()) {
        return false; // ID为空或已存在
    }
    
    teacher_map_.emplace(id, std::move(teacher));
    return true;
}

bool TeacherMgr::removeTeacher(const std::string& teacher_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = teacher_map_.find(teacher_id);
    if (it == teacher_map_.end()) {
        return false;
    }
    
    teacher_map_.erase(it);
    return true;
}

const TeacherBaseInfo* TeacherMgr::getTeacherById(const std::string& teacher_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = teacher_map_.find(teacher_id);
    return (it != teacher_map_.end()) ? it->second.get() : nullptr;
}

std::vector<const TeacherBaseInfo*> TeacherMgr::getAllTeachers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<const TeacherBaseInfo*> teachers;
    teachers.reserve(teacher_map_.size());
    
    for (const auto& pair : teacher_map_) {
        teachers.push_back(pair.second.get());
    }
    
    return teachers;
}

bool TeacherMgr::addCourseToTeacher(const std::string& teacher_id, const std::string& course_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = teacher_map_.find(teacher_id);
    if (it == teacher_map_.end()) {
        return false;
    }
    
    it->second->addCourse(course_id);
    return true;
}

bool TeacherMgr::removeCourseFromTeacher(const std::string& teacher_id, const std::string& course_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = teacher_map_.find(teacher_id);
    if (it == teacher_map_.end()) {
        return false;
    }
    
    it->second->removeCourse(course_id);
    return true;
}

std::vector<std::string> TeacherMgr::getCoursesByTeacher(const std::string& teacher_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = teacher_map_.find(teacher_id);
    if (it == teacher_map_.end()) {
        return {};
    }
    
    return it->second->getCoursesTaught();
}

} // namespace chenglei
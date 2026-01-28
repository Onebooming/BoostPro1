/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-27
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-26 21:11:41
 * @FilePath: /BoostPro1/master/server/src/entity/teacher.hpp
 * @Description: 教师个人信息类与管理类
 */
#pragma once
#include <string>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include "personbase.hpp"

namespace chenglei {

// 教师个人信息类
class TeacherBaseInfo : public PPBaseInfo {
public:
    // 构造函数
    TeacherBaseInfo() : PPBaseInfo() {}
    ~TeacherBaseInfo() override = default;

    // Getters 和 Setters
    const std::string& getDepartment() const { return department_; }
    void setDepartment(const std::string& department) { department_ = department; }

    const std::string& getPosition() const { return position_; }
    void setPosition(const std::string& position) { position_ = position; }

    const std::string& getOffice() const { return office_; }
    void setOffice(const std::string& office) { office_ = office; }

    const std::string& getTeacherID() const { return teacher_id_; }
    void setTeacherID(const std::string& teacher_id) { teacher_id_ = teacher_id; }

    const std::vector<std::string>& getCoursesTaught() const { return courses_taught_; }
    void addCourse(const std::string& course_id) { courses_taught_.push_back(course_id); }
    void removeCourse(const std::string& course_id);

private:
    std::string department_;      // 所属院系
    std::string position_;        // 职称
    std::string office_;          // 办公室位置
    std::string teacher_id_;      // 教师工号（全局唯一）
    std::vector<std::string> courses_taught_; // 所授课程ID列表
};

/*
    TeacherBaseInfo对象的全局管理器：单例模式
 */
class TeacherMgr {
public:
    static TeacherMgr& getInstance() {
        static TeacherMgr inst;
        return inst;
    }

    // 添加教师
    bool addTeacher(std::unique_ptr<TeacherBaseInfo> teacher);
    
    // 删除教师
    bool removeTeacher(const std::string& teacher_id);
    
    // 获取教师信息
    const TeacherBaseInfo* getTeacherById(const std::string& teacher_id) const;
    
    // 获取所有教师列表
    std::vector<const TeacherBaseInfo*> getAllTeachers() const;
    
    // 为教师添加课程
    bool addCourseToTeacher(const std::string& teacher_id, const std::string& course_id);
    
    // 为教师移除课程
    bool removeCourseFromTeacher(const std::string& teacher_id, const std::string& course_id);
    
    // 获取教师所授课程
    std::vector<std::string> getCoursesByTeacher(const std::string& teacher_id) const;

private:
    TeacherMgr() = default;
    TeacherMgr(const TeacherMgr&) = delete;
    TeacherMgr& operator=(const TeacherMgr&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<TeacherBaseInfo>> teacher_map_;
    mutable std::mutex mutex_; // 可变的互斥锁，用于const成员函数
};

} // namespace chenglei
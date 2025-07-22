/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-21 17:08:12
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-22 23:57:24
 * @FilePath: /BoostPro1/master/server/src/entity/student.hpp
 * @Description: student相关实例类
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

// 学生信息类
class StudentBaseInfo : public PPBaseInfo {
public:
    // 构造函数
    StudentBaseInfo(): PPBaseInfo() {}
    ~StudentBaseInfo() override = default;

    // Getters 和 Setters
    const std::string& getGrade() const { return grade_; }
    void setGrade(const std::string& grade) { grade_ = grade; }

    const std::string& getClassName() const { return class_name_; }
    void setClassName(const std::string& class_name) { class_name_ = class_name; }

    const std::string& getMajor() const { return major_; }
    void setMajor(const std::string& major) { major_ = major; }

    const std::string& getStudentID() const { return student_id_; }
    void setStudentID(const std::string& student_id) { student_id_ = student_id; }

private:
    std::string grade_;
    std::string class_name_;
    std::string major_;
    std::string student_id_; // 全局唯一
};

/*
    StudentBaseInfo对象的全局管理器：单例模式、通过std::unordered_map<std::string, std::unique_ptr<StudentBaseInfo>>容器管理
 */
class StudentMgr {
public:
    static StudentMgr& getInstance() {
        static StudentMgr inst;
        return inst;
    }

    unsigned int addStudent(std::unique_ptr<StudentBaseInfo> np_student);
    void removeStudentByStuId(std::string & student_id);
    const StudentBaseInfo * getStudentByStuId(std::string & student_id) const;
    std::vector<StudentBaseInfo *> getAllStudentList(void);

private:
    StudentMgr() = default;
    StudentMgr(const StudentMgr&) = delete;
    StudentMgr& operator=(const StudentMgr&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<StudentBaseInfo>> student_map_;
    std::mutex mutex_;
};


} // namespace chenglei
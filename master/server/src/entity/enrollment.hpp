#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "student.hpp"
#include "course.hpp"
#include "../utils/snowflake.hpp"

namespace chenglei {

// 选课状态枚举
enum class EnrollmentStatus {
    ENROLLED,    // 已选课
    DROPPED,     // 已退课
    COMPLETED    // 已完成
};

// 选课信息类
class Enrollment {
public:
    Enrollment(const std::string& student_id, const std::string& course_id);

    // 特殊成员函数
    Enrollment() = default;
    Enrollment(const Enrollment& other) = default;
    Enrollment(Enrollment&& other) noexcept = default;
    Enrollment& operator=(const Enrollment& other) = default;
    Enrollment& operator=(Enrollment&& other) noexcept = default;
    ~Enrollment() = default;

    // Getters
    const std::string& getId() const { return id_; }
    const std::string& getStudentId() const { return student_id_; }
    const std::string& getCourseId() const { return course_id_; }
    const std::string& getEnrollmentTime() const { return enrollment_time_; }
    EnrollmentStatus getStatus() const { return status_; }

    // Setters
    void setStatus(EnrollmentStatus status);
    void setEnrollmentTime(const std::string& time) { enrollment_time_ = time; }

    // 工具方法
    std::string getStatusString() const;
    void fromStatusString(const std::string& status_str);

    // JSON序列化
    nlohmann::json toJson() const;
    static Enrollment fromJson(const nlohmann::json& json);

private:
    std::string id_;
    std::string student_id_;
    std::string course_id_;
    std::string enrollment_time_;
    EnrollmentStatus status_;
};

// 选课管理器
class EnrollmentManager {
public:
    static EnrollmentManager& getInstance() {
        static EnrollmentManager instance;
        return instance;
    }

    // 选课
    bool enrollCourse(const std::string& student_id, const std::string& course_id);

    // 退课
    bool dropCourse(const std::string& student_id, const std::string& course_id);

    // 获取学生的选课列表
    std::vector<Enrollment> getStudentEnrollments(const std::string& student_id);

    // 获取课程的学生列表
    std::vector<Enrollment> getCourseEnrollments(const std::string& course_id);

    // 检查是否已选课
    bool isEnrolled(const std::string& student_id, const std::string& course_id);

    // 获取选课信息
    std::optional<Enrollment> getEnrollment(const std::string& student_id, const std::string& course_id);

    // 获取所有选课记录
    std::vector<Enrollment> getAllEnrollments();

    // 完成课程（课程结束后调用）
    bool completeCourse(const std::string& student_id, const std::string& course_id);

private:
    EnrollmentManager() = default;
    EnrollmentManager(const EnrollmentManager&) = delete;
    EnrollmentManager& operator=(const EnrollmentManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<Enrollment>> enrollments_;
    std::mutex mutex_;

    // 生成复合键
    std::string generateKey(const std::string& student_id, const std::string& course_id);
};

} // namespace chenglei
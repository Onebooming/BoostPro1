#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <optional>
#include "student.hpp"
#include "course.hpp"
#include "enrollment.hpp"
#include "../utils/snowflake.hpp"

namespace chenglei {

// 等级评定枚举
enum class GradeLevel {
    A_PLUS,  // A+
    A,       // A
    A_MINUS, // A-
    B_PLUS,  // B+
    B,       // B
    B_MINUS, // B-
    C_PLUS,  // C+
    C,       // C
    C_MINUS, // C-
    D,       // D
    F        // 不及格
};

// 成绩信息类
class Grade {
public:
    Grade(const std::string& student_id, const std::string& course_id, const std::string& enrollment_id);

    // 特殊成员函数
    Grade() = default;
    Grade(const Grade& other) = default;
    Grade(Grade&& other) noexcept = default;
    Grade& operator=(const Grade& other) = default;
    Grade& operator=(Grade&& other) noexcept = default;
    ~Grade() = default;

    // Getters
    const std::string& getId() const { return id_; }
    const std::string& getStudentId() const { return student_id_; }
    const std::string& getCourseId() const { return course_id_; }
    const std::string& getEnrollmentId() const { return enrollment_id_; }
    std::optional<double> getRegularScore() const { return regular_score_; }
    std::optional<double> getMidtermScore() const { return midterm_score_; }
    std::optional<double> getFinalScore() const { return final_score_; }
    std::optional<double> getTotalScore() const { return total_score_; }
    std::optional<double> getGpa() const { return gpa_; }
    std::optional<GradeLevel> getGradeLevel() const { return grade_level_; }
    const std::string& getRemarks() const { return remarks_; }
    const std::string& getGradedBy() const { return graded_by_; }
    const std::string& getGradedAt() const { return graded_at_; }

    // Setters
    void setRegularScore(double score);
    void setMidtermScore(double score);
    void setFinalScore(double score);
    void setTotalScore(double score);
    void calculateTotalScore(double regular_weight = 0.3, double midterm_weight = 0.3, double final_weight = 0.4);
    void calculateGpa();
    void setGradeLevel(GradeLevel level);
    void setRemarks(const std::string& remarks) { remarks_ = remarks; }
    void setGradedBy(const std::string& teacher_id) { graded_by_ = teacher_id; }
    void setGradedAt(const std::string& time) { graded_at_ = time; }

    // 工具方法
    std::string getGradeLevelString() const;
    void fromGradeLevelString(const std::string& level_str);
    bool isComplete() const;
    void finalizeGrading(const std::string& teacher_id);

    // JSON序列化
    nlohmann::json toJson() const;
    static Grade fromJson(const nlohmann::json& json);

private:
    std::string id_;
    std::string student_id_;
    std::string course_id_;
    std::string enrollment_id_;
    std::optional<double> regular_score_;    // 平时成绩
    std::optional<double> midterm_score_;    // 期中成绩
    std::optional<double> final_score_;      // 期末成绩
    std::optional<double> total_score_;      // 总成绩
    std::optional<double> gpa_;             // 绩点
    std::optional<GradeLevel> grade_level_;  // 等级评定
    std::string remarks_;                   // 备注
    std::string graded_by_;                 // 评分教师ID
    std::string graded_at_;                 // 评分时间

    // 工具方法
    static GradeLevel scoreToGradeLevel(double score);
    static double gradeLevelToGpa(GradeLevel level);
};

// 成绩管理器
class GradeManager {
public:
    static GradeManager& getInstance() {
        static GradeManager instance;
        return instance;
    }

    // 创建成绩记录
    bool createGradeRecord(const std::string& student_id, const std::string& course_id, const std::string& enrollment_id);

    // 更新成绩
    bool updateScores(const std::string& student_id, const std::string& course_id,
                     std::optional<double> regular_score = std::nullopt,
                     std::optional<double> midterm_score = std::nullopt,
                     std::optional<double> final_score = std::nullopt,
                     const std::string& remarks = "");

    // 计算并更新总成绩
    bool calculateTotalScore(const std::string& student_id, const std::string& course_id,
                            double regular_weight = 0.3, double midterm_weight = 0.3, double final_weight = 0.4);

    // 完成评分
    bool finalizeGrade(const std::string& student_id, const std::string& course_id, const std::string& teacher_id);

    // 获取成绩记录
    std::optional<Grade> getGrade(const std::string& student_id, const std::string& course_id);

    // 获取学生的所有成绩
    std::vector<Grade> getStudentGrades(const std::string& student_id);

    // 获取课程的所有成绩
    std::vector<Grade> getCourseGrades(const std::string& course_id);

    // 获取教师教授的课程成绩
    std::vector<Grade> getTeacherGrades(const std::string& teacher_id);

    // 获取成绩统计信息
    struct GradeStatistics {
        int total_students;
        double average_score;
        double highest_score;
        double lowest_score;
        std::unordered_map<GradeLevel, int> grade_distribution;
    };
    std::optional<GradeStatistics> getCourseGradeStatistics(const std::string& course_id);

    // 获取学生的GPA
    std::optional<double> calculateStudentGPA(const std::string& student_id);

    // 批量导入成绩
    bool batchImportGrades(const std::vector<Grade>& grades, const std::string& teacher_id);

private:
    GradeManager() = default;
    GradeManager(const GradeManager&) = delete;
    GradeManager& operator=(const GradeManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<Grade>> grades_;
    std::mutex mutex_;

    // 生成复合键
    std::string generateKey(const std::string& student_id, const std::string& course_id);
};

} // namespace chenglei
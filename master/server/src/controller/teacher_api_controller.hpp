#pragma once
#include "base_api_controller.hpp"
#include "../entity/teacher.hpp"
#include "../dao/teacher_dao.hpp"
#include <vector>
#include <optional>

namespace chenglei {

class TeacherApiController : public BaseApiController {
public:
    TeacherApiController() = default;
    virtual ~TeacherApiController() = default;

protected:
    http::response<http::string_body> handleGet(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params) override;

    http::response<http::string_body> handlePost(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params) override;

    http::response<http::string_body> handlePut(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params) override;

    http::response<http::string_body> handleDelete(
        const http::request<http::string_body>& req,
        const std::vector<std::string>& path_params) override;

private:
    // GET /api/teachers - 获取所有教师
    http::response<http::string_body> getAllTeachers(const http::request<http::string_body>& req);

    // GET /api/teachers/{id} - 根据ID获取教师
    http::response<http::string_body> getTeacherById(const std::string& id);

    // GET /api/teachers/department/{department} - 根据院系获取教师
    http::response<http::string_body> getTeachersByDepartment(const std::string& department);

    // GET /api/teachers/with-courses - 获取有课程的教师
    http::response<http::string_body> getTeachersWithCourses();

    // POST /api/teachers - 创建教师
    http::response<http::string_body> createTeacher(const http::request<http::string_body>& req);

    // PUT /api/teachers/{id} - 更新教师
    http::response<http::string_body> updateTeacher(const std::string& id,
                                                   const http::request<http::string_body>& req);

    // DELETE /api/teachers/{id} - 删除教师
    http::response<http::string_body> deleteTeacher(const std::string& id);

    // 工具方法
    nlohmann::json teacherToJson(const TeacherBaseInfo& teacher);
    TeacherBaseInfo jsonToTeacher(const nlohmann::json& json);
    bool validateTeacherData(const nlohmann::json& json, bool is_update = false);
};

} // namespace chenglei
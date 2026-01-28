/*
 * @FileName: classroom_dao.hpp
 * @Description: 教室数据访问接口
 */
#ifndef CLASSROOM_DAO_HPP
#define CLASSROOM_DAO_HPP

#include "../entity/classroom.hpp"
#include <string>
#include <vector>
#include <memory>

namespace chenglei {

class ClassroomDao {
public:
    static ClassroomDao& getInstance();

    // 创建表
    bool createTable();

    // 增加教室
    bool insertClassroom(const Classroom& classroom);

    // 更新教室
    bool updateClassroom(const Classroom& classroom);

    // 删除教室
    bool deleteClassroom(const std::string& id);

    // 根据ID查询教室
    std::unique_ptr<Classroom> selectClassroomById(const std::string& id);

    // 查询所有教室
    std::vector<Classroom> selectAllClassrooms();

    // 根据楼栋查询教室
    std::vector<Classroom> selectClassroomsByBuilding(const std::string& building);

    // 根据类型查询教室
    std::vector<Classroom> selectClassroomsByType(const std::string& type);

    // 根据状态查询教室
    std::vector<Classroom> selectClassroomsByStatus(const std::string& status);

private:
    ClassroomDao() = default;
    ~ClassroomDao() = default;
    ClassroomDao(const ClassroomDao&) = delete;
    ClassroomDao& operator=(const ClassroomDao&) = delete;

    // SQL转义
    std::string escapeSQL(const std::string& input);
};

} // namespace chenglei

#endif // CLASSROOM_DAO_HPP

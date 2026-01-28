#include "teacher_dao.hpp"
#include "../database/mysql_pro.hpp"
#include <mysql/mysql.h>
#include <iostream>
#include <sstream>

namespace chenglei {

TeacherBaseInfo TeacherDao::rowToTeacher(const MYSQL_ROW row) {
    TeacherBaseInfo teacher;
    teacher.setID(row[0] ? row[0] : "");
    teacher.setName(row[1] ? row[1] : "");
    teacher.setBirthDate(row[2] ? row[2] : "");
    teacher.setAddress(row[3] ? row[3] : "");
    teacher.setGender(row[4] ? row[4] : "");
    teacher.setHobby(row[5] ? row[5] : "");
    teacher.setPhoneNumber(row[6] ? row[6] : "");
    teacher.setFatherName(row[7] ? row[7] : "");
    teacher.setMotherName(row[8] ? row[8] : "");
    teacher.setDepartment(row[9] ? row[9] : "");
    teacher.setPosition(row[10] ? row[10] : "");
    teacher.setOffice(row[11] ? row[11] : "");
    teacher.setTeacherID(row[12] ? row[12] : "");

    // 注意：不再在这里获取课程，避免死锁
    // 课程需要在获取完所有教师后单独查询

    return teacher;
}

std::vector<std::string> TeacherDao::getCoursesByTeacherId(const std::string& teacher_id) {
    std::vector<std::string> courses;

    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return courses;

    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();
    std::string sql = "SELECT id FROM courses WHERE teacher_id='" + teacher_id + "'";

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Query teacher courses error: " << mysql_error(conn) << std::endl;
        return courses;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return courses;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        if (row[0]) {
            courses.push_back(row[0]);
        }
    }

    mysql_free_result(res);
    return courses;
}

bool TeacherDao::insertTeacher(const TeacherBaseInfo& teacher) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql =
        "INSERT INTO teachers (id, name, birth_date, address, gender, hobby, phone_number, father_name, mother_name, department, position, office, teacher_id) "
        "VALUES ('" + teacher.getID() + "', '" + teacher.getName() + "', '" + teacher.getBirthDate() + "', '" +
        teacher.getAddress() + "', '" + teacher.getGender() + "', '" + teacher.getHobby() + "', '" +
        teacher.getPhoneNumber() + "', '" + teacher.getFatherName() + "', '" + teacher.getMotherName() + "', '" +
        teacher.getDepartment() + "', '" + teacher.getPosition() + "', '" + teacher.getOffice() + "', '" +
        teacher.getTeacherID() + "')";

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Insert teacher error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

bool TeacherDao::deleteTeacherById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "DELETE FROM teachers WHERE id='" + id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Delete teacher error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

bool TeacherDao::updateTeacher(const TeacherBaseInfo& teacher) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql =
        "UPDATE teachers SET name='" + teacher.getName() +
        "', birth_date='" + teacher.getBirthDate() +
        "', address='" + teacher.getAddress() +
        "', gender='" + teacher.getGender() +
        "', hobby='" + teacher.getHobby() +
        "', phone_number='" + teacher.getPhoneNumber() +
        "', father_name='" + teacher.getFatherName() +
        "', mother_name='" + teacher.getMotherName() +
        "', department='" + teacher.getDepartment() +
        "', position='" + teacher.getPosition() +
        "', office='" + teacher.getOffice() +
        "', teacher_id='" + teacher.getTeacherID() +
        "' WHERE id='" + teacher.getID() + "'";

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Update teacher error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

std::optional<TeacherBaseInfo> TeacherDao::selectTeacherById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return std::nullopt;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT * FROM teachers WHERE id='" + id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select teacher error: " << mysql_error(conn) << std::endl;
        return std::nullopt;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return std::nullopt;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        TeacherBaseInfo teacher = rowToTeacher(row);
        mysql_free_result(res);
        return teacher;
    }

    mysql_free_result(res);
    return std::nullopt;
}

std::optional<TeacherBaseInfo> TeacherDao::selectTeacherByTeacherId(const std::string& teacher_id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return std::nullopt;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT * FROM teachers WHERE teacher_id='" + teacher_id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select teacher by teacher_id error: " << mysql_error(conn) << std::endl;
        return std::nullopt;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return std::nullopt;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        TeacherBaseInfo teacher = rowToTeacher(row);
        mysql_free_result(res);
        return teacher;
    }

    mysql_free_result(res);
    return std::nullopt;
}

std::vector<TeacherBaseInfo> TeacherDao::selectAllTeachers() {
    std::vector<TeacherBaseInfo> teachers;
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return teachers;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT * FROM teachers ORDER BY name";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select all teachers error: " << mysql_error(conn) << std::endl;
        return teachers;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return teachers;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        teachers.push_back(rowToTeacher(row));
    }

    mysql_free_result(res);
    return teachers;
}

std::vector<TeacherBaseInfo> TeacherDao::selectTeachersByDepartment(const std::string& department) {
    std::vector<TeacherBaseInfo> teachers;
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return teachers;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT * FROM teachers WHERE department='" + department + "' ORDER BY name";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select teachers by department error: " << mysql_error(conn) << std::endl;
        return teachers;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return teachers;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        teachers.push_back(rowToTeacher(row));
    }

    mysql_free_result(res);
    return teachers;
}

std::vector<TeacherBaseInfo> TeacherDao::selectTeachersWithCourses() {
    std::vector<TeacherBaseInfo> teachers;
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return teachers;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT DISTINCT t.* FROM teachers t "
                     "INNER JOIN courses c ON t.id = c.teacher_id "
                     "ORDER BY t.name";

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select teachers with courses error: " << mysql_error(conn) << std::endl;
        return teachers;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return teachers;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        teachers.push_back(rowToTeacher(row));
    }

    mysql_free_result(res);
    return teachers;
}

} // namespace chenglei
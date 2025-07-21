#include "student_dao.hpp"
#include "../database/mysql_pro.hpp"
#include <mysql/mysql.h>
#include <iostream>

namespace chenglei {

namespace {
// 工具：从MYSQL_ROW构造StudentBaseInfo
StudentBaseInfo row_to_student(const MYSQL_ROW row) {
    StudentBaseInfo s;
    s.setID(row[0] ? row[0] : "");
    s.setName(row[1] ? row[1] : "");
    s.setBirthDate(row[2] ? row[2] : "");
    s.setAddress(row[3] ? row[3] : "");
    s.setGender(row[4] ? row[4] : "");
    s.setHobby(row[5] ? row[5] : "");
    s.setPhoneNumber(row[6] ? row[6] : "");
    s.setFatherName(row[7] ? row[7] : "");
    s.setMotherName(row[8] ? row[8] : "");
    s.setGrade(row[9] ? row[9] : "");
    s.setClassName(row[10] ? row[10] : "");
    s.setMajor(row[11] ? row[11] : "");
    s.setStudentID(row[12] ? row[12] : "");
    return s;
}
}

// 增
bool StudentDao::insertStudent(const StudentBaseInfo& student) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    MYSQL* conn = conn_client->get();

    std::string sql =
        "INSERT INTO students (id, name, birth_date, address, gender, hobby, phone_number, father_name, mother_name, grade, class_name, major, student_id) "
        "VALUES ('" + student.getID() + "', '" + student.getName() + "', '" + student.getBirthDate() + "', '" +
        student.getAddress() + "', '" + student.getGender() + "', '" + student.getHobby() + "', '" +
        student.getPhoneNumber() + "', '" + student.getFatherName() + "', '" + student.getMotherName() + "', '" +
        student.getGrade() + "', '" + student.getClassName() + "', '" + student.getMajor() + "', '" +
        student.getStudentID() + "')";

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Insert Error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

// 删
bool StudentDao::deleteStudentById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    MYSQL* conn = conn_client->get();

    std::string sql = "DELETE FROM students WHERE id='" + id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Delete Error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

// 改
bool StudentDao::updateStudent(const StudentBaseInfo& student) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    MYSQL* conn = conn_client->get();

    std::string sql =
        "UPDATE students SET name='" + student.getName() +
        "', birth_date='" + student.getBirthDate() +
        "', address='" + student.getAddress() +
        "', gender='" + student.getGender() +
        "', hobby='" + student.getHobby() +
        "', phone_number='" + student.getPhoneNumber() +
        "', father_name='" + student.getFatherName() +
        "', mother_name='" + student.getMotherName() +
        "', grade='" + student.getGrade() +
        "', class_name='" + student.getClassName() +
        "', major='" + student.getMajor() +
        "', student_id='" + student.getStudentID() +
        "' WHERE id='" + student.getID() + "'";

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Update Error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

// 查单个
std::optional<StudentBaseInfo> StudentDao::selectStudentById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return std::nullopt;
    MYSQL* conn = conn_client->get();

    std::string sql = "SELECT * FROM students WHERE id='" + id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select Error: " << mysql_error(conn) << std::endl;
        return std::nullopt;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return std::nullopt;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        StudentBaseInfo stu = row_to_student(row);
        mysql_free_result(res);
        return stu;
    }
    mysql_free_result(res);
    return std::nullopt;
}

// 查全部
std::vector<StudentBaseInfo> StudentDao::selectAllStudents() {
    std::vector<StudentBaseInfo> vec;
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return vec;
    MYSQL* conn = conn_client->get();

    std::string sql = "SELECT * FROM students";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select Error: " << mysql_error(conn) << std::endl;
        return vec;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return vec;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        vec.push_back(row_to_student(row));
    }
    mysql_free_result(res);
    return vec;
}

// 按学号查
std::optional<StudentBaseInfo> StudentDao::selectStudentByStudentId(const std::string& student_id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return std::nullopt;
    MYSQL* conn = conn_client->get();

    std::string sql = "SELECT * FROM students WHERE student_id='" + student_id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select Error: " << mysql_error(conn) << std::endl;
        return std::nullopt;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return std::nullopt;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        StudentBaseInfo stu = row_to_student(row);
        mysql_free_result(res);
        return stu;
    }
    mysql_free_result(res);
    return std::nullopt;
}

// 按手机号查
std::optional<StudentBaseInfo> StudentDao::selectStudentByPhone(const std::string& phone_number) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return std::nullopt;
    MYSQL* conn = conn_client->get();

    std::string sql = "SELECT * FROM students WHERE phone_number='" + phone_number + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select Error: " << mysql_error(conn) << std::endl;
        return std::nullopt;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return std::nullopt;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        StudentBaseInfo stu = row_to_student(row);
        mysql_free_result(res);
        return stu;
    }
    mysql_free_result(res);
    return std::nullopt;
}

} // namespace chenglei
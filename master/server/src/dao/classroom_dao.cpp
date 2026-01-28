/*
 * @FileName: classroom_dao.cpp
 * @Description: 教室数据访问实现
 */
#include "classroom_dao.hpp"
#include "database/mysql_pro.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace chenglei {

ClassroomDao& ClassroomDao::getInstance() {
    static ClassroomDao instance;
    return instance;
}

std::string ClassroomDao::escapeSQL(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c == '\'' || c == '\\' || c == '"') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

bool ClassroomDao::createTable() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) {
        std::cerr << "Failed to get MySQL connection" << std::endl;
        return false;
    }
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS classrooms (
            id VARCHAR(64) PRIMARY KEY,
            room_number VARCHAR(50) NOT NULL UNIQUE,
            building VARCHAR(50) NOT NULL,
            floor INT DEFAULT 1,
            capacity INT DEFAULT 40,
            room_type VARCHAR(50) DEFAULT '普通教室',
            has_projector BOOLEAN DEFAULT FALSE,
            has_computer BOOLEAN DEFAULT FALSE,
            has_audio BOOLEAN DEFAULT FALSE,
            status VARCHAR(20) DEFAULT '可用',
            remark TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            INDEX idx_building (building),
            INDEX idx_type (room_type),
            INDEX idx_status (status)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to create classrooms table: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool ClassroomDao::insertClassroom(const Classroom& classroom) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::stringstream ss;
    ss << "INSERT INTO classrooms (id, room_number, building, floor, capacity, room_type, "
       << "has_projector, has_computer, has_audio, status, remark) VALUES ('"
       << classroom.id << "', '"
       << escapeSQL(classroom.room_number) << "', '"
       << escapeSQL(classroom.building) << "', "
       << classroom.floor << ", "
       << classroom.capacity << ", '"
       << escapeSQL(classroom.room_type) << "', "
       << (classroom.has_projector ? 1 : 0) << ", "
       << (classroom.has_computer ? 1 : 0) << ", "
       << (classroom.has_audio ? 1 : 0) << ", '"
       << escapeSQL(classroom.status) << "', '"
       << escapeSQL(classroom.remark) << "')";

    if (mysql_query(mysql, ss.str().c_str())) {
        std::cerr << "Failed to insert classroom: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool ClassroomDao::updateClassroom(const Classroom& classroom) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::stringstream ss;
    ss << "UPDATE classrooms SET "
       << "room_number = '" << escapeSQL(classroom.room_number) << "', "
       << "building = '" << escapeSQL(classroom.building) << "', "
       << "floor = " << classroom.floor << ", "
       << "capacity = " << classroom.capacity << ", "
       << "room_type = '" << escapeSQL(classroom.room_type) << "', "
       << "has_projector = " << (classroom.has_projector ? 1 : 0) << ", "
       << "has_computer = " << (classroom.has_computer ? 1 : 0) << ", "
       << "has_audio = " << (classroom.has_audio ? 1 : 0) << ", "
       << "status = '" << escapeSQL(classroom.status) << "', "
       << "remark = '" << escapeSQL(classroom.remark) << "' "
       << "WHERE id = '" << classroom.id << "'";

    if (mysql_query(mysql, ss.str().c_str())) {
        std::cerr << "Failed to update classroom: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool ClassroomDao::deleteClassroom(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "DELETE FROM classrooms WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to delete classroom: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

std::unique_ptr<Classroom> ClassroomDao::selectClassroomById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return nullptr;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM classrooms WHERE id = '" + escapeSQL(id) + "'";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query classroom: " << mysql_error(mysql) << std::endl;
        return nullptr;
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return nullptr;

    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Classroom> classroom;

    if (row) {
        classroom = std::make_unique<Classroom>();
        classroom->id = row[0] ? row[0] : "";
        classroom->room_number = row[1] ? row[1] : "";
        classroom->building = row[2] ? row[2] : "";
        classroom->floor = row[3] ? atoi(row[3]) : 1;
        classroom->capacity = row[4] ? atoi(row[4]) : 40;
        classroom->room_type = row[5] ? row[5] : "普通教室";
        classroom->has_projector = row[6] ? (atoi(row[6]) != 0) : false;
        classroom->has_computer = row[7] ? (atoi(row[7]) != 0) : false;
        classroom->has_audio = row[8] ? (atoi(row[8]) != 0) : false;
        classroom->status = row[9] ? row[9] : "可用";
        classroom->remark = row[10] ? row[10] : "";
        classroom->created_at = row[11] ? row[11] : "";
        classroom->updated_at = row[12] ? row[12] : "";
    }

    mysql_free_result(result);
    return classroom;
}

std::vector<Classroom> ClassroomDao::selectAllClassrooms() {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM classrooms ORDER BY building, room_number";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query classrooms: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Classroom> classrooms;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Classroom classroom;
        classroom.id = row[0] ? row[0] : "";
        classroom.room_number = row[1] ? row[1] : "";
        classroom.building = row[2] ? row[2] : "";
        classroom.floor = row[3] ? atoi(row[3]) : 1;
        classroom.capacity = row[4] ? atoi(row[4]) : 40;
        classroom.room_type = row[5] ? row[5] : "普通教室";
        classroom.has_projector = row[6] ? (atoi(row[6]) != 0) : false;
        classroom.has_computer = row[7] ? (atoi(row[7]) != 0) : false;
        classroom.has_audio = row[8] ? (atoi(row[8]) != 0) : false;
        classroom.status = row[9] ? row[9] : "可用";
        classroom.remark = row[10] ? row[10] : "";
        classroom.created_at = row[11] ? row[11] : "";
        classroom.updated_at = row[12] ? row[12] : "";

        classrooms.push_back(classroom);
    }

    mysql_free_result(result);
    return classrooms;
}

std::vector<Classroom> ClassroomDao::selectClassroomsByBuilding(const std::string& building) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM classrooms WHERE building = '" + escapeSQL(building) + "' ORDER BY room_number";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query classrooms by building: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Classroom> classrooms;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Classroom classroom;
        classroom.id = row[0] ? row[0] : "";
        classroom.room_number = row[1] ? row[1] : "";
        classroom.building = row[2] ? row[2] : "";
        classroom.floor = row[3] ? atoi(row[3]) : 1;
        classroom.capacity = row[4] ? atoi(row[4]) : 40;
        classroom.room_type = row[5] ? row[5] : "普通教室";
        classroom.has_projector = row[6] ? (atoi(row[6]) != 0) : false;
        classroom.has_computer = row[7] ? (atoi(row[7]) != 0) : false;
        classroom.has_audio = row[8] ? (atoi(row[8]) != 0) : false;
        classroom.status = row[9] ? row[9] : "可用";
        classroom.remark = row[10] ? row[10] : "";
        classroom.created_at = row[11] ? row[11] : "";
        classroom.updated_at = row[12] ? row[12] : "";

        classrooms.push_back(classroom);
    }

    mysql_free_result(result);
    return classrooms;
}

std::vector<Classroom> ClassroomDao::selectClassroomsByType(const std::string& type) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM classrooms WHERE room_type = '" + escapeSQL(type) + "' ORDER BY building, room_number";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query classrooms by type: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Classroom> classrooms;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Classroom classroom;
        classroom.id = row[0] ? row[0] : "";
        classroom.room_number = row[1] ? row[1] : "";
        classroom.building = row[2] ? row[2] : "";
        classroom.floor = row[3] ? atoi(row[3]) : 1;
        classroom.capacity = row[4] ? atoi(row[4]) : 40;
        classroom.room_type = row[5] ? row[5] : "普通教室";
        classroom.has_projector = row[6] ? (atoi(row[6]) != 0) : false;
        classroom.has_computer = row[7] ? (atoi(row[7]) != 0) : false;
        classroom.has_audio = row[8] ? (atoi(row[8]) != 0) : false;
        classroom.status = row[9] ? row[9] : "可用";
        classroom.remark = row[10] ? row[10] : "";
        classroom.created_at = row[11] ? row[11] : "";
        classroom.updated_at = row[12] ? row[12] : "";

        classrooms.push_back(classroom);
    }

    mysql_free_result(result);
    return classrooms;
}

std::vector<Classroom> ClassroomDao::selectClassroomsByStatus(const std::string& status) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return {};
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* mysql = locked_conn.get();

    std::string sql = "SELECT * FROM classrooms WHERE status = '" + escapeSQL(status) + "' ORDER BY building, room_number";

    if (mysql_query(mysql, sql.c_str())) {
        std::cerr << "Failed to query classrooms by status: " << mysql_error(mysql) << std::endl;
        return {};
    }

    MYSQL_RES* result = mysql_store_result(mysql);
    if (!result) return {};

    std::vector<Classroom> classrooms;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        Classroom classroom;
        classroom.id = row[0] ? row[0] : "";
        classroom.room_number = row[1] ? row[1] : "";
        classroom.building = row[2] ? row[2] : "";
        classroom.floor = row[3] ? atoi(row[3]) : 1;
        classroom.capacity = row[4] ? atoi(row[4]) : 40;
        classroom.room_type = row[5] ? row[5] : "普通教室";
        classroom.has_projector = row[6] ? (atoi(row[6]) != 0) : false;
        classroom.has_computer = row[7] ? (atoi(row[7]) != 0) : false;
        classroom.has_audio = row[8] ? (atoi(row[8]) != 0) : false;
        classroom.status = row[9] ? row[9] : "可用";
        classroom.remark = row[10] ? row[10] : "";
        classroom.created_at = row[11] ? row[11] : "";
        classroom.updated_at = row[12] ? row[12] : "";

        classrooms.push_back(classroom);
    }

    mysql_free_result(result);
    return classrooms;
}

} // namespace chenglei

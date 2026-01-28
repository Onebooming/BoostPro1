#include "course_dao.hpp"
#include "../database/mysql_pro.hpp"
#include <mysql/mysql.h>
#include <iostream>
#include <sstream>

namespace chenglei {

CourseDetailed CourseDao::rowToCourse(MYSQL* conn, const MYSQL_ROW row, bool with_schedules, bool with_teacher_info) {
    CourseDetailed course;
    course.id = row[0] ? row[0] : "";
    course.course_id = row[1] ? row[1] : "";
    course.course_name = row[2] ? row[2] : "";
    course.teacher_id = row[3] ? row[3] : "";
    course.credits = row[4] ? std::stod(row[4]) : 0.0;
    course.max_students = row[5] ? std::stoi(row[5]) : 0;
    course.current_students = row[6] ? std::stoi(row[6]) : 0;
    course.course_type = row[7] ? row[7] : "";
    course.semester = row[8] ? row[8] : "";
    course.description = row[9] ? row[9] : "";
    course.created_at = row[10] ? row[10] : "";
    course.updated_at = row[11] ? row[11] : "";

    if (with_teacher_info) {
        course.teacher_name = getTeacherName(conn, course.teacher_id);
    }

    if (with_schedules) {
        course.schedules = getSchedulesForCourse(conn, course.id);
    }

    return course;
}

std::vector<CourseDetailed::CourseSchedule> CourseDao::getSchedulesForCourse(MYSQL* conn, const std::string& course_id) {
    std::vector<CourseDetailed::CourseSchedule> schedules;

    std::string sql = "SELECT day_of_week, start_time, end_time, location, week_range "
                     "FROM course_schedules WHERE course_id='" + course_id + "' "
                     "ORDER BY day_of_week, start_time";

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Query course schedules error: " << mysql_error(conn) << std::endl;
        return schedules;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return schedules;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        CourseDetailed::CourseSchedule schedule;
        schedule.day_of_week = row[0] ? row[0] : "";
        schedule.start_time = row[1] ? row[1] : "";
        schedule.end_time = row[2] ? row[2] : "";
        schedule.location = row[3] ? row[3] : "";
        schedule.week_range = row[4] ? row[4] : "";
        schedules.push_back(schedule);
    }

    mysql_free_result(res);
    return schedules;
}

std::string CourseDao::getTeacherName(MYSQL* conn, const std::string& teacher_id) {
    std::string sql = "SELECT name FROM teachers WHERE id='" + teacher_id + "'";

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Query teacher name error: " << mysql_error(conn) << std::endl;
        return "";
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return "";

    MYSQL_ROW row = mysql_fetch_row(res);
    std::string name = row && row[0] ? row[0] : "";
    mysql_free_result(res);

    return name;
}

bool CourseDao::insertCourse(const CourseDetailed& course) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::ostringstream sql;
    sql << "INSERT INTO courses (id, course_id, course_name, teacher_id, credits, max_students, "
        << "current_students, course_type, semester, description) VALUES ('"
        << course.id << "', '" << course.course_id << "', '" << course.course_name << "', '"
        << course.teacher_id << "', " << course.credits << ", " << course.max_students << ", "
        << course.current_students << ", '" << course.course_type << "', '" << course.semester
        << "', '" << course.description << "')";

    int ret = mysql_query(conn, sql.str().c_str());
    if (ret != 0) {
        std::cerr << "Insert course error: " << mysql_error(conn) << std::endl;
        return false;
    }

    // 插入课程时间安排
    for (const auto& schedule : course.schedules) {
        if (!insertCourseSchedule(course.id, schedule)) {
            std::cerr << "Insert course schedule failed" << std::endl;
        }
    }

    return true;
}

bool CourseDao::deleteCourseById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    // 先删除时间安排
    deleteCourseSchedules(id);

    // 删除课程
    std::string sql = "DELETE FROM courses WHERE id='" + id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Delete course error: " << mysql_error(conn) << std::endl;
        return false;
    }

    return true;
}

bool CourseDao::updateCourse(const CourseDetailed& course) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::ostringstream sql;
    sql << "UPDATE courses SET course_name='" << course.course_name
        << "', teacher_id='" << course.teacher_id
        << "', credits=" << course.credits
        << ", max_students=" << course.max_students
        << ", course_type='" << course.course_type
        << "', semester='" << course.semester
        << "', description='" << course.description
        << "' WHERE id='" << course.id << "'";

    int ret = mysql_query(conn, sql.str().c_str());
    if (ret != 0) {
        std::cerr << "Update course error: " << mysql_error(conn) << std::endl;
        return false;
    }

    // 更新时间安排：先删除旧的，再插入新的
    deleteCourseSchedules(course.id);
    for (const auto& schedule : course.schedules) {
        insertCourseSchedule(course.id, schedule);
    }

    return true;
}

std::optional<CourseDetailed> CourseDao::selectCourseById(const std::string& id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return std::nullopt;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT * FROM courses WHERE id='" + id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select course error: " << mysql_error(conn) << std::endl;
        return std::nullopt;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return std::nullopt;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        CourseDetailed course = rowToCourse(conn, row, true, true);
        mysql_free_result(res);
        return course;
    }

    mysql_free_result(res);
    return std::nullopt;
}

std::optional<CourseDetailed> CourseDao::selectCourseByCourseId(const std::string& course_id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return std::nullopt;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT * FROM courses WHERE course_id='" + course_id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select course by course_id error: " << mysql_error(conn) << std::endl;
        return std::nullopt;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return std::nullopt;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        CourseDetailed course = rowToCourse(conn, row, true, true);
        mysql_free_result(res);
        return course;
    }

    mysql_free_result(res);
    return std::nullopt;
}

std::vector<CourseDetailed> CourseDao::selectAllCourses() {
    std::vector<CourseDetailed> courses;
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return courses;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT * FROM courses ORDER BY course_name";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select all courses error: " << mysql_error(conn) << std::endl;
        return courses;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return courses;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        courses.push_back(rowToCourse(conn, row, false, true));
    }

    mysql_free_result(res);
    return courses;
}

std::vector<CourseDetailed> CourseDao::selectCoursesByTeacher(const std::string& teacher_id) {
    std::vector<CourseDetailed> courses;
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return courses;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "SELECT * FROM courses WHERE teacher_id='" + teacher_id + "' ORDER BY course_name";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Select courses by teacher error: " << mysql_error(conn) << std::endl;
        return courses;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return courses;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        courses.push_back(rowToCourse(conn, row, false, false));
    }

    mysql_free_result(res);
    return courses;
}

std::string CourseDao::buildWhereClause(const CourseQueryParams& params) {
    std::vector<std::string> conditions;

    if (!params.teacher_id.empty()) {
        conditions.push_back("teacher_id='" + params.teacher_id + "'");
    }

    if (!params.course_type.empty()) {
        conditions.push_back("course_type='" + params.course_type + "'");
    }

    if (!params.semester.empty()) {
        conditions.push_back("semester='" + params.semester + "'");
    }

    if (!params.keyword.empty()) {
        conditions.push_back("(course_name LIKE '%" + params.keyword + "%' OR course_id LIKE '%" + params.keyword + "%')");
    }

    if (conditions.empty()) {
        return "";
    }

    std::string where_clause = "WHERE ";
    for (size_t i = 0; i < conditions.size(); ++i) {
        if (i > 0) where_clause += " AND ";
        where_clause += conditions[i];
    }

    return where_clause;
}

std::string CourseDao::buildOrderClause(const CourseQueryParams& params) {
    return "ORDER BY course_name";
}

std::vector<CourseDetailed> CourseDao::searchCourses(const CourseQueryParams& params) {
    std::vector<CourseDetailed> courses;
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return courses;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string where_clause = buildWhereClause(params);
    std::string order_clause = buildOrderClause(params);
    std::string limit_clause = "LIMIT " + std::to_string((params.page - 1) * params.page_size) +
                               ", " + std::to_string(params.page_size);

    std::string sql = "SELECT * FROM courses " + where_clause + " " + order_clause + " " + limit_clause;

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Search courses error: " << mysql_error(conn) << std::endl;
        return courses;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return courses;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        courses.push_back(rowToCourse(conn, row, params.with_schedules, params.with_teacher_info));
    }

    mysql_free_result(res);
    return courses;
}

int CourseDao::countCourses(const CourseQueryParams& params) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return 0;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string where_clause = buildWhereClause(params);
    std::string sql = "SELECT COUNT(*) FROM courses " + where_clause;

    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Count courses error: " << mysql_error(conn) << std::endl;
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return 0;

    MYSQL_ROW row = mysql_fetch_row(res);
    int count = row && row[0] ? std::stoi(row[0]) : 0;

    mysql_free_result(res);
    return count;
}

bool CourseDao::insertCourseSchedule(const std::string& course_id, const CourseDetailed::CourseSchedule& schedule) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string schedule_id = SnowflakeIDGenerator::instance().next_id();

    std::ostringstream sql;
    sql << "INSERT INTO course_schedules (id, course_id, day_of_week, start_time, end_time, location, week_range) "
        << "VALUES ('" << schedule_id << "', '" << course_id << "', '" << schedule.day_of_week
        << "', '" << schedule.start_time << "', '" << schedule.end_time << "', '"
        << schedule.location << "', '" << schedule.week_range << "')";

    int ret = mysql_query(conn, sql.str().c_str());
    if (ret != 0) {
        std::cerr << "Insert course schedule error: " << mysql_error(conn) << std::endl;
        return false;
    }

    return true;
}

bool CourseDao::deleteCourseSchedules(const std::string& course_id) {
    auto conn_client = MySQLConnectionManager::instance().getConnection("boostpro1", "127.0.0.1", "root", "root", 3306);
    if (!conn_client) return false;
    // 使用RAII锁守卫，在整个操作期间持有锁
    auto locked_conn = conn_client->getLocked();
    MYSQL* conn = locked_conn.get();

    std::string sql = "DELETE FROM course_schedules WHERE course_id='" + course_id + "'";
    int ret = mysql_query(conn, sql.c_str());
    if (ret != 0) {
        std::cerr << "Delete course schedules error: " << mysql_error(conn) << std::endl;
        return false;
    }

    return true;
}

} // namespace chenglei
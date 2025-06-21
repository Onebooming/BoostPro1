#include "student_dao.hpp"
#include <iostream>

namespace chenglei {

StudentDAO::StudentDAO() : connection_(nullptr) {
    if (!connectToDatabase()) {
        std::cerr << "Failed to connect to database!" << std::endl;
    }
}

StudentDAO::~StudentDAO() {
    closeConnection();
}

bool StudentDAO::connectToDatabase() {
    connection_ = mysql_init(nullptr);
    if (!connection_) {
        std::cerr << "MySQL initialization failed!" << std::endl;
        return false;
    }

    // Replace with your database credentials
    if (!mysql_real_connect(connection_, "localhost", "root", "password", "student_db", 3306, nullptr, 0)) {
        std::cerr << "MySQL connection error: " << mysql_error(connection_) << std::endl;
        return false;
    }
    return true;
}

void StudentDAO::closeConnection() {
    if (connection_) {
        mysql_close(connection_);
        connection_ = nullptr;
    }
}

bool StudentDAO::addStudent(const StudentBaseInfo& student) {
    // 检查表是否存在，如果不存在则创建
    std::string checkTableQuery = "SHOW TABLES LIKE 'students'";
    if (mysql_query(connection_, checkTableQuery.c_str())) {
        std::cerr << "Check table existence failed: " << mysql_error(connection_) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(connection_);
    if (!result) {
        std::cerr << "Check table result error: " << mysql_error(connection_) << std::endl;
        return false;
    }

    if (mysql_num_rows(result) == 0) {
        // 表不存在，创建表
        std::string createTableQuery = R"(
            CREATE TABLE students (
                id VARCHAR(255) PRIMARY KEY,
                name VARCHAR(255),
                birth_date DATE,
                address VARCHAR(255),
                gender VARCHAR(50),
                hobby VARCHAR(255),
                phone_number VARCHAR(50),
                father_name VARCHAR(255),
                mother_name VARCHAR(255),
                grade VARCHAR(50),
                class_name VARCHAR(50),
                major VARCHAR(255),
                student_id VARCHAR(255) UNIQUE
            )
        )";
        if (mysql_query(connection_, createTableQuery.c_str())) {
            std::cerr << "Create table failed: " << mysql_error(connection_) << std::endl;
            mysql_free_result(result);
            return false;
        }
    }
    mysql_free_result(result);

    // 插入学生信息
    std::string query = "INSERT INTO students (id, name, birth_date, address, gender, hobby, phone_number, father_name, mother_name, grade, class_name, major, student_id) VALUES ('" +
                        student.getID() + "', '" + student.getName() + "', '" + student.getBirthDate() + "', '" + student.getAddress() + "', '" + student.getGender() + "', '" +
                        student.getHobby() + "', '" + student.getPhoneNumber() + "', '" + student.getFatherName() + "', '" + student.getMotherName() + "', '" + student.getGrade() +
                        "', '" + student.getClassName() + "', '" + student.getMajor() + "', '" + student.getStudentID() + "')";
    if (mysql_query(connection_, query.c_str())) {
        std::cerr << "Add student failed: " << mysql_error(connection_) << std::endl;
        return false;
    }
    return true;
}

bool StudentDAO::deleteStudent(const std::string& student_id) {
    std::string query = "DELETE FROM students WHERE student_id = '" + student_id + "'";
    if (mysql_query(connection_, query.c_str())) {
        std::cerr << "Delete student failed: " << mysql_error(connection_) << std::endl;
        return false;
    }
    return true;
}

bool StudentDAO::updateStudent(const StudentBaseInfo& student) {
    std::string query = "UPDATE students SET name = '" + student.getName() + "', birth_date = '" + student.getBirthDate() + "', address = '" + student.getAddress() +
                        "', gender = '" + student.getGender() + "', hobby = '" + student.getHobby() + "', phone_number = '" + student.getPhoneNumber() +
                        "', father_name = '" + student.getFatherName() + "', mother_name = '" + student.getMotherName() + "', grade = '" + student.getGrade() +
                        "', class_name = '" + student.getClassName() + "', major = '" + student.getMajor() + "' WHERE student_id = '" + student.getStudentID() + "'";
    if (mysql_query(connection_, query.c_str())) {
        std::cerr << "Update student failed: " << mysql_error(connection_) << std::endl;
        return false;
    }
    return true;
}

StudentBaseInfo StudentDAO::getStudent(const std::string& student_id) {
    std::string query = "SELECT * FROM students WHERE student_id = '" + student_id + "'";
    StudentBaseInfo student;

    if (mysql_query(connection_, query.c_str())) {
        std::cerr << "Get student failed: " << mysql_error(connection_) << std::endl;
        return student;
    }

    MYSQL_RES* result = mysql_store_result(connection_);
    if (!result) {
        std::cerr << "Get student result error: " << mysql_error(connection_) << std::endl;
        return student;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        student.setID(row[0]);
        student.setName(row[1]);
        student.setBirthDate(row[2]);
        student.setAddress(row[3]);
        student.setGender(row[4]);
        student.setHobby(row[5]);
        student.setPhoneNumber(row[6]);
        student.setFatherName(row[7]);
        student.setMotherName(row[8]);
        student.setGrade(row[9]);
        student.setClassName(row[10]);
        student.setMajor(row[11]);
        student.setStudentID(row[12]);
    }

    mysql_free_result(result);
    return student;
}

} // namespace chenglei
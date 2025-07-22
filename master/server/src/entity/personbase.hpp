#pragma once
/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-21 17:08:12
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-22 23:34:29
 * @FilePath: /BoostPro1/master/server/src/entity/personbase.hpp
 * @Description: 人物信息的基本类
 */
#pragma once
#include <string>
#include <atomic>
#include "../utils/snowflake.hpp"

namespace chenglei {

// 基础信息类
class PPBaseInfo {
public:
    // 构造函数
    PPBaseInfo()
    {
        std::string global_id = chenglei::SnowflakeIDGenerator::instance().next_id();
        id_ = global_id;
    }
    virtual ~PPBaseInfo() = default;

    // Getters 和 Setters
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    const std::string& getBirthDate() const { return birth_date_; }
    void setBirthDate(const std::string& birth_date) { birth_date_ = birth_date; }

    const std::string& getAddress() const { return address_; }
    void setAddress(const std::string& address) { address_ = address; }

    const std::string& getID() const { return id_; }
    void setID(const std::string& id) { id_ = id; }

    const std::string& getGender() const { return gender_; }
    void setGender(const std::string& gender) { gender_ = gender; }

    const std::string& getHobby() const { return hobby_; }
    void setHobby(const std::string& hobby) { hobby_ = hobby; }

    const std::string& getPhoneNumber() const { return phone_number_; }
    void setPhoneNumber(const std::string& phone_number) { phone_number_ = phone_number; }

    const std::string& getFatherName() const { return father_name_; }
    void setFatherName(const std::string& father_name) { father_name_ = father_name; }

    const std::string& getMotherName() const { return mother_name_; }
    void setMotherName(const std::string& mother_name) { mother_name_ = mother_name; }

private:
    std::string name_;
    std::string birth_date_;
    std::string address_;
    std::string id_; // 全局唯一
    std::string gender_;
    std::string hobby_;
    std::string phone_number_;
    std::string father_name_;
    std::string mother_name_;
};


} // namespace chenglei
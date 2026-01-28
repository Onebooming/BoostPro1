/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-26 12:23:08
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-26 22:05:05
 * @FilePath: /BoostPro1/master/server/src/utils/DateTimeUtil.hpp
 * @Description: 日期、时间工具类
 */
#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <ctime>

namespace chenglei {

enum Weekday {
    SUNDAY,
    MONDAY = 1,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,

};

static const std::vector<std::string> WEEKDAYS = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};


/* 时间工具类 - 处理时间转换和格式化 */
class DateTimeUtil {
public:

/* 字符串转时间点 (格式: "YYYY/MM/DD HH:MM") */
static std::chrono::system_clock::time_point stringToTimePoint(const std::string& datatime_str) {
    std::tm tm = {};
    std::istringstream ss(datatime_str);
    ss >> std::get_time(&tm, "%Y/%m/%d %H:%M");
    if (ss.fail()) {
        throw std::invalid_argument("Invalid datetime format. Expected: YYYY/MM/DD HH:MM");
    }
    tm.tm_isdst = -1; // 自动判断夏令时
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

/* 时间点转日期字符串 (格式: "YYYY/MM/DD") */ 
static std::string timePointToDateString(const std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y/%m/%d");
    return oss.str();
}

/* 时间点转时间字符串 (格式: "HH:MM") */
static std::string timePointToTimeString(const std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M");
    return oss.str();
}

// 解析时间范围字符串 (格式: "HH:MM-HH:MM")
static std::pair<std::string, std::string> parseTimeRange(const std::string& time_range) {
    size_t pos = time_range.find('-');
    if (pos == std::string::npos || pos == 0 || pos == time_range.length() - 1) {
        throw std::invalid_argument("Invalid time range format. Expected: HH:MM-HH:MM");
    }
    return {time_range.substr(0, pos), time_range.substr(pos + 1)};
}

static std::string getWeekDayString(enum Weekday& enum_weekday) {
    return WEEKDAYS.at(static_cast<int>(enum_weekday));
}

// 新增函数
static std::string timePointToString(const std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y/%m/%d %H:%M");
    return oss.str();
}

};
}
#include "course_detailed.hpp"

namespace chenglei {

nlohmann::json CourseDetailed::toJson() const {
    nlohmann::json j;
    j["id"] = id;
    j["course_id"] = course_id;
    j["course_name"] = course_name;
    j["teacher_id"] = teacher_id;
    j["teacher_name"] = teacher_name;
    j["credits"] = credits;
    j["max_students"] = max_students;
    j["current_students"] = current_students;
    j["course_type"] = course_type;
    j["semester"] = semester;
    j["description"] = description;
    j["created_at"] = created_at;
    j["updated_at"] = updated_at;

    // 序列化课程安排
    nlohmann::json schedulesArray = nlohmann::json::array();
    for (const auto& schedule : schedules) {
        nlohmann::json scheduleJson;
        scheduleJson["day_of_week"] = schedule.day_of_week;
        scheduleJson["start_time"] = schedule.start_time;
        scheduleJson["end_time"] = schedule.end_time;
        scheduleJson["location"] = schedule.location;
        scheduleJson["week_range"] = schedule.week_range;
        schedulesArray.push_back(scheduleJson);
    }
    j["schedules"] = schedulesArray;

    return j;
}

CourseDetailed CourseDetailed::fromJson(const nlohmann::json& json) {
    CourseDetailed course;

    if (json.contains("id")) course.id = json["id"];
    if (json.contains("course_id")) course.course_id = json["course_id"];
    if (json.contains("course_name")) course.course_name = json["course_name"];
    if (json.contains("teacher_id")) course.teacher_id = json["teacher_id"];
    if (json.contains("teacher_name")) course.teacher_name = json["teacher_name"];
    if (json.contains("credits")) course.credits = json["credits"];
    if (json.contains("max_students")) course.max_students = json["max_students"];
    if (json.contains("current_students")) course.current_students = json["current_students"];
    if (json.contains("course_type")) course.course_type = json["course_type"];
    if (json.contains("semester")) course.semester = json["semester"];
    if (json.contains("description")) course.description = json["description"];
    if (json.contains("created_at")) course.created_at = json["created_at"];
    if (json.contains("updated_at")) course.updated_at = json["updated_at"];

    // 反序列化课程安排
    if (json.contains("schedules") && json["schedules"].is_array()) {
        for (const auto& scheduleJson : json["schedules"]) {
            CourseSchedule schedule;
            if (scheduleJson.contains("day_of_week")) schedule.day_of_week = scheduleJson["day_of_week"];
            if (scheduleJson.contains("start_time")) schedule.start_time = scheduleJson["start_time"];
            if (scheduleJson.contains("end_time")) schedule.end_time = scheduleJson["end_time"];
            if (scheduleJson.contains("location")) schedule.location = scheduleJson["location"];
            if (scheduleJson.contains("week_range")) schedule.week_range = scheduleJson["week_range"];
            course.schedules.push_back(schedule);
        }
    }

    return course;
}

} // namespace chenglei

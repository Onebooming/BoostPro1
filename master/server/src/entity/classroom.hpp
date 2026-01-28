/*
 * @FileName: classroom.hpp
 * @Description: 教室实体类
 */
#ifndef CLASSROOM_HPP
#define CLASSROOM_HPP

#include <string>
#include <ctime>

namespace chenglei {

/**
 * @brief 教室实体类
 */
class Classroom {
public:
    std::string id;              // 教室ID（雪花ID）
    std::string room_number;     // 教室编号，如A101
    std::string building;        // 楼栋，如A栋、B栋
    int floor;                   // 楼层
    int capacity;                // 容量（人数）
    std::string room_type;       // 教室类型（普通教室、多媒体教室、实验室、计算机房）
    bool has_projector;          // 是否有投影仪
    bool has_computer;           // 是否有计算机
    bool has_audio;              // 是否有音响设备
    std::string status;          // 状态（可用、维修中、占用）
    std::string remark;          // 备注
    std::string created_at;      // 创建时间
    std::string updated_at;      // 更新时间

    /**
     * @brief 构造函数
     */
    Classroom() : floor(1), capacity(40), has_projector(false), has_computer(false), has_audio(false) {}

    /**
     * @brief 获取当前时间
     */
    static std::string getCurrentTime() {
        time_t now = time(0);
        char buf[80];
        struct tm* nowtm = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", nowtm);
        return std::string(buf);
    }
};

} // namespace chenglei

#endif // CLASSROOM_HPP

/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-21 22:58:20
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-21 22:58:24
 * @FilePath: /BoostPro1/master/server/src/util/snowflake.hpp
 * @Description: 雪花算法ID生成器
 */

#pragma once
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <sstream>

namespace chenglei {

class SnowflakeIDGenerator {
public:
    static SnowflakeIDGenerator& instance() {
        static SnowflakeIDGenerator inst(1, 1); // 可以自定义datacenterId和workerId
        return inst;
    }

    // 生成下一个ID
    std::string next_id() {
        std::lock_guard<std::mutex> lock(mutex_);
        uint64_t timestamp = get_timestamp();
        if (timestamp == last_timestamp_) {
            sequence_ = (sequence_ + 1) & sequence_mask_;
            if (sequence_ == 0) {
                // 等到下一个毫秒
                timestamp = wait_next_millis(last_timestamp_);
            }
        } else {
            sequence_ = 0;
        }
        last_timestamp_ = timestamp;

        uint64_t id = ((timestamp - twepoch_) << timestamp_left_shift_)
                    | (datacenter_id_ << datacenter_id_shift_)
                    | (worker_id_ << worker_id_shift_)
                    | sequence_;
        return std::to_string(id);
    }

private:
    SnowflakeIDGenerator(int datacenter_id, int worker_id)
        : datacenter_id_(datacenter_id), worker_id_(worker_id),
          sequence_(0), last_timestamp_(0) {}

    uint64_t get_timestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    uint64_t wait_next_millis(uint64_t last_timestamp) {
        uint64_t ts = get_timestamp();
        while (ts <= last_timestamp) {
            ts = get_timestamp();
        }
        return ts;
    }

    static const uint64_t twepoch_ = 1721000000000ULL; // 可自定义起始时间戳
    static const int worker_id_bits_ = 5;
    static const int datacenter_id_bits_ = 5;
    static const int sequence_bits_ = 12;

    static const int worker_id_shift_ = sequence_bits_;
    static const int datacenter_id_shift_ = sequence_bits_ + worker_id_bits_;
    static const int timestamp_left_shift_ = sequence_bits_ + worker_id_bits_ + datacenter_id_bits_;
    static const uint64_t sequence_mask_ = -1LL ^ (-1LL << sequence_bits_);

    int datacenter_id_;
    int worker_id_;
    uint64_t sequence_;
    uint64_t last_timestamp_;
    std::mutex mutex_;
};

} // namespace chenglei
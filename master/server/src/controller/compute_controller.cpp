/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-14 19:48:52
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-14 20:25:10
 * @FilePath: /BoostPro1/master/server/src/controller/compute_controller.cpp
 * @Description: 计算任务控制器实现
 */
#include "compute_controller.hpp"
#include <string>
#include <thread>
#include <chrono>

namespace chenglei {

std::string ComputeSumController::method_process(const nlohmann::json &req) {
    int sum = 0;
        try {
            for (auto& v : req["param"]["data"]) sum += v.get<int>();
            nlohmann::json resp;
            resp["reponse"] = sum;
            return resp.dump();
        } catch (const std::exception& e) {
            return std::string(R"({"error":")") + e.what() + R"("})";
        }
}

std::string FactorialControler::method_process(const nlohmann::json& req) {
    try {
        if (!req.contains("param") || !req["param"].contains("number"))
            return R"({"error":"missing param.number"})";
        int n = req["param"]["number"];
        if (n < 0) return R"({"error":"number must be non-negative"})";
        if (n > 20) return R"({"error":"number too large, max 20"})"; // 防止溢出

        uint64_t result = 1;
        for (int i = 2; i <= n; ++i) result *= i;

        // --- 增加阻塞2秒 ---
        std::this_thread::sleep_for(std::chrono::seconds(2));

        nlohmann::json resp;
        resp["reponse"] = result;
        return resp.dump();
    } catch (const std::exception& e) {
        return std::string(R"({"error":")") + e.what() + R"("})";
    }
}

}
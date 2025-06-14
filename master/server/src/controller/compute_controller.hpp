/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-14 19:45:56
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-14 19:48:02
 * @FilePath: /BoostPro1/master/server/src/controller/compute_controller.hpp
 * @Description: 计算任务
 */
#pragma once
#include "base_controller.hpp"

namespace chenglei {

class ComputeSumController : public BaseController {
public:
    std::string method_process(const nlohmann::json &req) override;
};

class FactorialControler : public BaseController {
public:
    std::string method_process(const nlohmann::json &req) override;
};

}
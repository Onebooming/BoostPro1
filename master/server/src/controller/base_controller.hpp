/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-14 19:08:46
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-14 19:12:10
 * @FilePath: /BoostPro1/master/server/src/controller/base_controller.hpp
 * @Description: controller 虚基类
 */
#pragma once

#include<string>
#include "../../../public/json.hpp"

namespace chenglei {

class BaseController {
public:
    virtual ~BaseController() = default;

    virtual std::string method_process(const nlohmann::json &req) = 0;
};
}
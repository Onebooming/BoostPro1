/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-14 19:08:46
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-22 22:34:23
 * @FilePath: /BoostPro1/master/server/src/controller/base_controller.hpp
 * @Description: controller 虚基类
 */
#pragma once



#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include<string>
#include "../../../public/json.hpp"

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

namespace chenglei {

class BaseController {
public:
    virtual ~BaseController() = default;

    virtual int controller_process(http::request<http::string_body>& request, http::response<http::string_body>& response) = 0;
};
}
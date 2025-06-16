/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-15 13:15:47
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-16 21:37:41
 * @FilePath: /BoostPro1/master/server/src/urldispatch/baseurl_handler.hpp
 * @Description: 虚基类，定义url_handler
 */
#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

namespace chenglei {

class BaseUrlHandler {
public:
    virtual int url_handler(http::request<http::string_body>& request, http::response<http::string_body>& response) = 0;
};

}

/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-16 21:39:36
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-22 22:22:29
 * @FilePath: /BoostPro1/master/server/src/urldispatch/firststage_json_url_handler.hpp
 * @Description: /first_stage/json_methos URL对应的处理类
 */
#pragma once

#include "baseurl_handler.hpp"

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

namespace chenglei {

class ApiUrlHandler : public BaseUrlHandler {
public:
    ApiUrlHandler();
    int url_handler(http::request<http::string_body> &request, http::response<http::string_body> &response) override;
};
}
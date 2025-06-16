/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-16 21:39:36
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-16 22:06:54
 * @FilePath: /BoostPro1/master/server/src/urldispatch/firststage_html_url_handler.hpp
 * @Description: /first_stage/htmls URL对应的处理类
 */
#pragma once

#include "baseurl_handler.hpp"

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

namespace chenglei {

class FirstStageHtmlHelloWordUH : public BaseUrlHandler {
public:
    int url_handler(http::request<http::string_body> &request, http::response<http::string_body> &response) override;
};
}
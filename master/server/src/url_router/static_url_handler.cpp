/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-16 21:40:21
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-22 22:29:33
 * @FilePath: /BoostPro1/master/server/src/urldispatch/static_url_handler.cpp
 * @Description: /static URL对应的处理类
 */
#include "static_url_handler.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "../../../public/json.hpp"

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http; // from <boost/beast/http.hpp>
using json = nlohmann::json;

namespace chenglei {

// 读取整个文件到字符串
std::string read_file_to_string(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file) return ""; // 或抛异常/返回错误页面
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

int StaticUrlHandler::url_handler(http::request<http::string_body> &request, http::response<http::string_body> &response) {
    const std::string prefix = "/static/";
    std::string url = request.target().to_string();

    // 检查前缀
    if ((url.compare(0, prefix.size(), prefix) != 0) && (url.compare("//") != 0)){
        response.version(request.version());
        response.result(http::status::not_found);
        response.set(http::field::server, "Boost.Beast");
        response.set(http::field::content_type, "text/plain");
        response.body() = "404 Not Found";
        response.content_length(response.body().size());
        response.keep_alive(request.keep_alive());
        return 0;
    }

    // 获取html名
    std::string file_name = url.substr(prefix.size());
    if (file_name.empty()) file_name = "index.html"; // 允许 /first_stage/htmls/ 映射到 static/index.html
    std::string file_path = "static/" + file_name;

    std::string html_content = read_file_to_string(file_path);
    if (html_content.empty()) {
        response.version(request.version());
        response.result(http::status::not_found);
        response.set(http::field::server, "Boost.Beast");
        response.set(http::field::content_type, "text/plain");
        response.body() = "404 Not Found";
        response.content_length(response.body().size());
        response.keep_alive(request.keep_alive());
    } else {
        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, "Boost.Beast");
        response.set(http::field::content_type, "text/html; charset=utf-8");
        response.body() = html_content;
        response.content_length(response.body().size());
        response.keep_alive(request.keep_alive());
    }
    return 0;
}

}
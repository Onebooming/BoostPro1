/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-07 22:41:48
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-21 21:51:57
 * @FilePath: /BoostPro1/master/server/src/urldispatch/student_url_handler.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "student_url_handler.hpp"
#include <sstream>
#include <iostream>
namespace chenglei {

int StudentUrlHandler::url_handler(http::request<http::string_body>& request,
                                http::response<http::string_body> &response) {

    StudentController controller;
    std::string result_json;

    if (request.method() == http::verb::get) {
        // 解析参数
        auto target = request.target().to_string();
        auto pos = target.find('?');
        std::string id, name;
        if (pos != std::string::npos) {
            std::string query = target.substr(pos + 1);
            std::istringstream iss(query);
            std::string kv;
            while (std::getline(iss, kv, '&')) {
                auto eq = kv.find('=');
                if (eq != std::string::npos) {
                    auto key = kv.substr(0, eq);
                    auto value = kv.substr(eq + 1);
                    if (key == "id") id = value;
                    if (key == "name") name = value;
                }
            }
        }
        std::cout << "get method.\n";
        result_json = controller.query_student(id, name);
    } else if (request.method() == http::verb::post){
        result_json = controller.add_student(request.body());
        std::cout << "post method.\n";
    } else {
        response.result(http::status::bad_request);
        response.body() = "{\"error\":\"unsupported method\"}";
        std::cout << "other method.\n";
        return 0;
    }

    response.result(http::status::ok);
    response.set(http::field::content_type, "application/json");
    response.body() = result_json;
    response.content_length(response.body().size());
    return 0;
}

}
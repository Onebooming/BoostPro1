/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-07-22 22:19:23
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-07-22 22:53:07
 * @FilePath: /BoostPro1/master/server/src/urldispatch/api_url_handler.cpp
 * @Description: restful api相关请求处理路由类 /api
 */
#include "api_url_handler.hpp"

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

#include "../../../public/json.hpp"
#include "../api_router/api_router.hpp"
#include "../controller/student_controller.hpp"

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http; // from <boost/beast/http.hpp>
using json = nlohmann::json;


namespace chenglei {

void init_json_api_router() {
    chenglei::ApiRouterMgr::getInstance().registerController(
                                    "student", 
                                    std::make_unique<chenglei::StudentController>());
    // 可继续注册其他method
}

ApiUrlHandler::ApiUrlHandler() {
    init_json_api_router();
}

int ApiUrlHandler::url_handler(http::request<http::string_body> &request, http::response<http::string_body> &response) {
    int errcode = 0;
    std::string error_msg;

    // 解析请求体
    std::string url = request.target().to_string();
    /*
        url格式为 /api/method_name/xxx/...
        写函数获取method_name
    */
    std::string method;
    size_t pos = url.find('/', 5); // 跳过 "/api/"
    if (pos != std::string::npos) {
        method = url.substr(5, pos - 5); // 获取method_name
    } else {
        method = url.substr(5); // 如果没有后续部分，直接获取
    }

    try {
        auto* ctrl = chenglei::ApiRouterMgr::getInstance().getController(method);
        if (ctrl) {
            return ctrl->controller_process(request, response);
        } else {
            errcode = 1; // 未找到对应的控制器
        }
    } catch (const std::exception& e) {
        errcode = 2; // 处理异常
        error_msg = e.what();
    }

    if(errcode == 1) {
        response.result(http::status::not_found);
        response.set(http::field::content_type, "application/json");
        response.body() = R"({"error":"Method not found"})";
    }else if (errcode == 2)  // 处理异常
    {
        response.result(http::status::internal_server_error);
        response.set(http::field::content_type, "application/json");
        response.body() = std::string(R"({"error":")") + error_msg + R"("})";
    } 
    
    return 0;
}

}
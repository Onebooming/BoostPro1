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
#include "../controller/teacher_controller.hpp"
#include "../controller/course_controller.hpp"
#include "../controller/classroom_controller.hpp"
#include "../controller/news_controller.hpp"
#include "../controller/notification_controller_wrapper.hpp"

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http; // from <boost/beast/http.hpp>
using json = nlohmann::json;


namespace chenglei {

void init_json_api_router() {
    chenglei::ApiRouterMgr::getInstance().registerController(
                                    "student",
                                    std::make_unique<chenglei::StudentController>());
    chenglei::ApiRouterMgr::getInstance().registerController(
                                    "teacher",
                                    std::make_unique<chenglei::TeacherController>());
    chenglei::ApiRouterMgr::getInstance().registerController(
                                    "course",
                                    std::make_unique<chenglei::CourseController>());
    chenglei::ApiRouterMgr::getInstance().registerController(
                                    "classroom",
                                    std::make_unique<chenglei::ClassroomController>());
    chenglei::ApiRouterMgr::getInstance().registerController(
                                    "news",
                                    std::make_unique<chenglei::NewsController>());
    // 暂时禁用通知控制器，待MySQL连接问题修复后启用
    // chenglei::ApiRouterMgr::getInstance().registerController(
    //                                 "notification",
    //                                 std::make_unique<chenglei::NotificationControllerWrapper>());
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
    std::cout << "[API Handler] Received request: " << url << std::endl;

    /*
        url格式为 /api/method_name/xxx/... 或 /api/method_name?query=...
        写函数获取method_name
    */
    std::string method;
    size_t pos = url.find('/', 5); // 跳过 "/api/"
    size_t query_pos = url.find('?'); // 查找查询参数开始位置

    if (pos != std::string::npos) {
        // 有路径参数，如 /api/teacher/123
        method = url.substr(5, pos - 5); // 获取method_name
    } else {
        // 没有路径参数
        if (query_pos != std::string::npos) {
            // 有查询参数，如 /api/teacher?id=1
            method = url.substr(5, query_pos - 5); // 获取method_name，不包括查询参数
        } else {
            // 没有任何参数，如 /api/teacher
            method = url.substr(5);
        }
    }

    std::cout << "[API Handler] Parsed method: '" << method << "'" << std::endl;

    try {
        auto* ctrl = chenglei::ApiRouterMgr::getInstance().getController(method);
        if (ctrl) {
            std::cout << "[API Handler] Found controller, processing..." << std::endl;
            int result = ctrl->controller_process(request, response);
            std::cout << "[API Handler] Controller processed, result=" << result << std::endl;
            std::cout << "[API Handler] Response body: " << response.body().substr(0, 200) << "..." << std::endl;
            return result;
        } else {
            std::cout << "[API Handler] ERROR: Controller not found for method '" << method << "'" << std::endl;
            errcode = 1; // 未找到对应的控制器
        }
    } catch (const std::exception& e) {
        std::cout << "[API Handler] EXCEPTION: " << e.what() << std::endl;
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
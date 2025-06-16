#include "firststage_json_url_handler.hpp"

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
#include "../router/method_router.hpp"
#include "../controller/compute_controller.hpp"

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http; // from <boost/beast/http.hpp>
using json = nlohmann::json;


namespace chenglei {

void init_json_method_router() {
    chenglei::MethodRouter::getInstance().registerController("project_compute_data_sum", 
                        std::make_unique<chenglei::ComputeSumController>());
    chenglei::MethodRouter::getInstance().registerController("project_compute_factorial", 
                        std::make_unique<chenglei::FactorialControler>());
    // 可继续注册其他method
}

FirstStageJsonUrlHandler::FirstStageJsonUrlHandler() {
    init_json_method_router();
}

// 统一的请求处理函数
std::string handle_request(const std::string& body) {
    try {
        auto req_json = json::parse(body);
        std::string method = req_json["method"];
        auto* ctrl = chenglei::MethodRouter::getInstance().getController(method);
        if (ctrl) {
            return ctrl->method_process(req_json);
        } else {
            return R"({"error":"Unknown method"})";
        }
    } catch (const std::exception& e) {
        return std::string(R"({"error":")") + e.what() + R"("})";
    }
}

int FirstStageJsonUrlHandler::url_handler(http::request<http::string_body> &request, http::response<http::string_body> &response) {
    std::string result = handle_request(request.body());
    std::cout << "Received request: " << request.body() << std::endl;
    std::cout << "Sending response: " << result << std::endl;

    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "Boost.Beast");
    response.set(http::field::content_type, "application/json");
    response.body() = result;
    response.content_length(response.body().size());
    response.keep_alive(request.keep_alive());

    return 0;
}

}
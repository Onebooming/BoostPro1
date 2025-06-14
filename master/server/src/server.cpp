/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-14 18:55:50
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-14 22:01:12
 * @FilePath: /BoostPro1/master/server/src/server.cpp
 * @Description: http服务器主程序
 */
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

#include "../../public/basedata.hpp"
#include "../../public/json.hpp"
#include "router/method_roiuter.hpp"
#include "controller/compute_controller.hpp"

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http; // from <boost/beast/http.hpp>
using json = nlohmann::json;

std::mutex print_mutex;

// 读取整个文件到字符串
std::string read_file_to_string(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file) return ""; // 或抛异常/返回错误页面
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
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

class session : public std::enable_shared_from_this<session> {
    tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;

public:
    explicit session(tcp::socket socket)
        : socket_(std::move(socket)) {}

    void start() { do_read(); }

    void do_read() {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, req_,
            [self](boost::beast::error_code ec, std::size_t) {
                if (!ec) self->process_request();
            });
    }

    void process_request() {
        // 处理/first_stage/htmls?helloworld，返回html页面
        if (req_.target() == "/first_stage/htmls/helloworld") {
            std::string html_content = read_file_to_string("static/helloworld.html");
            if (html_content.empty()) {
                // 文件未找到，返回404
                res_.version(req_.version());
                res_.result(http::status::not_found);
                res_.set(http::field::server, "Boost.Beast");
                res_.set(http::field::content_type, "text/plain");
                res_.body() = "404 Not Found";
                res_.content_length(res_.body().size());
                res_.keep_alive(req_.keep_alive());
            } else {
                // 返回文件内容
                res_.version(req_.version());
                res_.result(http::status::ok);
                res_.set(http::field::server, "Boost.Beast");
                res_.set(http::field::content_type, "text/html; charset=utf-8");
                res_.body() = html_content;
                res_.content_length(res_.body().size());
                res_.keep_alive(req_.keep_alive());
            }
            auto self = shared_from_this();
            http::async_write(socket_, res_,
                [self](boost::beast::error_code ec, std::size_t) {
                    self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                });
            return;
        }

        // 只允许 /first_stage/json_methos 处理method请求
        if (req_.target() != "/first_stage/json_methos") {
            res_.version(req_.version());
            res_.result(http::status::not_found);
            res_.set(http::field::server, "Boost.Beast");
            res_.set(http::field::content_type, "application/json");
            res_.body() = R"({"error":"Invalid URL, only /first_stage/json_methos or /first_stage/htmls?helloworld are supported."})";
            res_.content_length(res_.body().size());
            res_.keep_alive(req_.keep_alive());

            auto self = shared_from_this();
            http::async_write(socket_, res_,
                [self](boost::beast::error_code ec, std::size_t) {
                    self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                });
            return;
        }

        std::string result = handle_request(req_.body());
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "Received request: " << req_.body() << std::endl;
        std::cout << "Sending response: " << result << std::endl;

        res_.version(req_.version());
        res_.result(http::status::ok);
        res_.set(http::field::server, "Boost.Beast");
        res_.set(http::field::content_type, "application/json");
        res_.body() = result;
        res_.content_length(res_.body().size());
        res_.keep_alive(req_.keep_alive());

        auto self = shared_from_this();
        http::async_write(socket_, res_,
            [self](boost::beast::error_code ec, std::size_t) {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
            });
    }
};

class listener : public std::enable_shared_from_this<listener> {
    boost::asio::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    listener(boost::asio::io_context& ioc, tcp::endpoint endpoint)
        : ioc_(ioc), acceptor_(ioc) {
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
    }

    void run() { do_accept(); }

    void do_accept() {
        acceptor_.async_accept(
            boost::asio::make_strand(ioc_),
            [self=shared_from_this()](boost::beast::error_code ec, tcp::socket socket) {
                if (!ec) std::make_shared<session>(std::move(socket))->start();
                self->do_accept();
            });
    }
};

void init_router() {
    chenglei::MethodRouter::getInstance().registerController("project_compute_data_sum", 
                        std::make_unique<chenglei::ComputeSumController>());
    chenglei::MethodRouter::getInstance().registerController("project_compute_factorial", 
                        std::make_unique<chenglei::FactorialControler>());
    // 可继续注册其他method
}

int main() {
    init_router();
    auto const address = boost::asio::ip::make_address(HTTP_SERVER_ADDR);
    unsigned short port = HTTP_PORT;
    boost::asio::io_context ioc{};

    std::make_shared<listener>(ioc, tcp::endpoint{address, port})->run();
    std::cout << "HTTP server started at port " << port << std::endl;

    // 启动N个线程并发执行io_context
    const int thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&ioc] { ioc.run(); });
    }
    for (auto& t : threads) t.join();
    return 0;
}
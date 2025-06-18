/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-14 18:55:50
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-18 23:56:11
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
#include "urldispatch/url_router.hpp"
#include "urldispatch/firststage_html_url_handler.hpp"
#include "urldispatch/firststage_json_url_handler.hpp"
#include "database/mysql_pro.hpp"

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http; // from <boost/beast/http.hpp>
using json = nlohmann::json;

std::mutex print_mutex;



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
        std::string url = req_.target().to_string();
        auto urlhandler = chenglei::UrlRouter::instance().get(url);
        if(urlhandler) {
            // 如果是html请求，直接调用html处理器
            (void)urlhandler->url_handler(req_, res_);
        }
        else {
            res_.version(req_.version());
            res_.result(http::status::not_found);
            res_.set(http::field::server, "Boost.Beast");
            res_.set(http::field::content_type, "text/plain");
            res_.body() = "Invalid Url";
            res_.content_length(res_.body().size());
            res_.keep_alive(req_.keep_alive());
        }

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

void init_url_router() {
    // 注册URL处理器
    chenglei::UrlRouter::instance().registerUrl("/first_stage/htmls",
        std::make_unique<chenglei::FirstStageHtmlHelloWordUH>());
    chenglei::UrlRouter::instance().registerUrl("/first_stage/json_methos",
        std::make_unique<chenglei::FirstStageJsonUrlHandler>());
    
    // 可以继续注册其他URL处理器
}

int main() {
    init_mysql_module();
    init_url_router();

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

    finish_mysql_module();
    
    return 0;
}
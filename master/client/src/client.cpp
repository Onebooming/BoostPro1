/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-14 18:57:50
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-15 09:12:19
 * @FilePath: /BoostPro1/master/client/src/client.cpp
 * @Description: Boost.Beast HTTP 客户端示例
 */
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

std::string host = "127.0.0.1";
std::string port = "20080";

int test_project_compute_data_sum(void) {
    try {
        std::string target = "/first_stage/json_methos";
        int version = 11;

        // 构造请求体
        std::string json_body = R"({"method":"project_compute_data_sum","param":{"data":[1,2,4,6,9]}})";

        // io context
        net::io_context ioc;

        // 解析域名
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 构造HTTP请求
        http::request<http::string_body> req{http::verb::post, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.body() = json_body;
        req.prepare_payload();

        // 发送请求
        http::write(stream, req);

        // 读取响应
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        std::cout << "Server Response:\n" << res.body() << std::endl;

        // 关闭socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

std::mutex print_mutex;

void send_factorial_request(int number, int thread_id) {
    try {
        std::string target = "/first_stage/json_methos";
        int version = 11;

        // 构造请求体
        std::string json_body = "{\"method\":\"project_compute_factorial\",\"param\":{\"number\":" + std::to_string(number) + "}}";

        net::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        http::request<http::string_body> req{http::verb::post, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.body() = json_body;
        req.prepare_payload();

        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "Thread " << thread_id << " got response: " << res.body() << std::endl;
        }

        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    } catch (std::exception const& e) {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cerr << "Thread " << thread_id << " error: " << e.what() << std::endl;
    }
}

int test_project_compute_factorial() {
    const int threads = 10;
    std::vector<std::thread> ths;

    for (int i = 0; i < threads; ++i) {
        ths.emplace_back(send_factorial_request, 20, i+1);
    }

    for (auto& t : ths) t.join();

    return 0;
}

int test_project_helloworld_html() {
    try {

        int version = 11;

        net::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 你给的请求
        http::request<http::empty_body> req{http::verb::get, "/first_stage/htmls/helloworld", version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        std::cout << "Response code: " << res.result_int() << std::endl;
        std::cout << "Content-Type: " << res[http::field::content_type] << std::endl;
        std::cout << "Body: " << std::endl << res.body() << std::endl;

        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    } catch(std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int main() {
    (void)test_project_compute_data_sum();

    (void)test_project_compute_factorial();

    (void)test_project_helloworld_html();

    return 0;
}
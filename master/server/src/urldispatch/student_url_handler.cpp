#include "student_url_handler.hpp"
#include <sstream>
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
        result_json = controller.query_student(id, name);
    } else if (request.method() == http::verb::post){
        result_json = controller.add_student(request.body());
    } else {
        response.result(http::status::bad_request);
        response.body() = "{\"error\":\"unsupported method\"}";
        return 0;
    }

    response.result(http::status::ok);
    response.set(http::field::content_type, "application/json");
    response.body() = result_json;
    response.content_length(response.body().size());
    return 0;
}

}
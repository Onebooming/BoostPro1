/*
 * @FileName: notification_controller_wrapper.hpp
 * @Description: 通知控制器包装类，继承BaseController
 */
#ifndef NOTIFICATION_CONTROLLER_WRAPPER_HPP
#define NOTIFICATION_CONTROLLER_WRAPPER_HPP

#include "base_controller.hpp"
#include "notification_controller.hpp"

namespace chenglei {

class NotificationControllerWrapper : public BaseController {
private:
    NotificationController notificationController;

public:
    int controller_process(http::request<http::string_body>& request,
                          http::response<http::string_body>& response) override {
        // 解析请求
        std::string requestBody = request.body();
        std::string url = request.target().to_string();

        // 提取action参数
        std::string action;
        size_t actionPos = url.find("action=");
        if (actionPos != std::string::npos) {
            size_t start = actionPos + 7; // "action="的长度
            size_t end = url.find('&', start);
            if (end == std::string::npos) {
                action = url.substr(start);
            } else {
                action = url.substr(start, end - start);
            }
        }

        // 处理请求
        http::response<http::string_body> res =
            notificationController.handleRequest(request, action, requestBody);

        // 复制响应
        response = res;
        return 0;
    }
};

} // namespace chenglei

#endif // NOTIFICATION_CONTROLLER_WRAPPER_HPP

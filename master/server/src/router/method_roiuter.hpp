#ifndef METHOD_ROUTER_HPP
#define METHOD_ROUTER_HPP
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "../controller/base_controller.hpp"

namespace chenglei {
class MethodRouter {
public:
    static MethodRouter& getInstance() {
        static MethodRouter instance;
        return instance;
    }

    void registerController(const std::string &method, std::unique_ptr<chenglei::BaseController> controller);

    chenglei::BaseController* getController(const std::string &method);

private:
    MethodRouter() = default;
    MethodRouter(const MethodRouter&) = delete;
    MethodRouter& operator=(const MethodRouter&) = delete;
    std::unordered_map<std::string, std::shared_ptr<BaseController>> controllers_;
    std::mutex mutex_;

};

}

#endif // METHOD_ROUTER_HPP

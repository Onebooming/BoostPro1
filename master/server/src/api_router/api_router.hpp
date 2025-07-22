#ifndef METHOD_ROUTER_HPP
#define METHOD_ROUTER_HPP
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "../controller/base_controller.hpp"

namespace chenglei {
class ApiRouterMgr {
public:
    static ApiRouterMgr& getInstance() {
        static ApiRouterMgr instance;
        return instance;
    }

    void registerController(const std::string &method, std::unique_ptr<chenglei::BaseController> controller);

    chenglei::BaseController* getController(const std::string &method);

private:
    ApiRouterMgr() = default;
    ApiRouterMgr(const ApiRouterMgr&) = delete;
    ApiRouterMgr& operator=(const ApiRouterMgr&) = delete;
    std::unordered_map<std::string, std::shared_ptr<BaseController>> controllers_;
    std::mutex mutex_;

};

}

#endif // METHOD_ROUTER_HPP

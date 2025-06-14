#include "method_roiuter.hpp"

namespace chenglei {

void MethodRouter::registerController(const std::string &method, 
                std::unique_ptr<chenglei::BaseController> controller){
    std::lock_guard<std::mutex> lock(mutex_);
    if (controllers_.find(method) != controllers_.end()) {
        throw std::runtime_error("Controller for method '" + method + "' already registered.");
    }
    controllers_[method] = std::move(controller);
}

chenglei::BaseController* MethodRouter::getController(const std::string &method){
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = controllers_.find(method);
    if (it != controllers_.end()) {
        return it->second.get();
    }
    return nullptr; // 如果没有找到对应的控制器，返回nullptr
}

}


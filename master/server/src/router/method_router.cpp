/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-14 20:27:53
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-16 21:47:01
 * @FilePath: /BoostPro1/master/server/src/router/method_router.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "method_router.hpp"

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


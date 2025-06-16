/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-15 13:20:03
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-16 22:34:40
 * @FilePath: /BoostPro1/master/server/src/urldispatch/url_router.cpp
 * @Description: url路由器实现类
 */
#include "url_router.hpp"

namespace chenglei {

    void UrlRouter::registerUrl(const std::string& url, std::unique_ptr<BaseUrlHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_[url] = std::move(handler);
    }

BaseUrlHandler* UrlRouter::get(const std::string& request_url) {
    if (request_url.empty() || request_url[0] != '/') return nullptr;
    std::lock_guard<std::mutex> lock(mutex_);

    BaseUrlHandler* best = nullptr;
    size_t best_len = 0;
    for (const auto& [prefix, handler_ptr] : handlers_) {
        // prefix 必须是 request_url 的前缀，且下一个字符是/或结尾，防止误匹配
        if (request_url.compare(0, prefix.size(), prefix) == 0) {
            // 精确匹配或 prefix 后面跟 /
            if (request_url.size() == prefix.size() ||
                request_url[prefix.size()] == '/') {
                if (prefix.size() > best_len) {
                    best = handler_ptr.get();
                    best_len = prefix.size();
                }
            }
        }
    }
    return best;
}
}
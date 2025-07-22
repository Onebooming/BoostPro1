/*
 * @Author: Onebooming 1026781822@qq.com
 * @Date: 2025-06-15 13:20:14
 * @LastEditors: Onebooming 1026781822@qq.com
 * @LastEditTime: 2025-06-15 21:20:28
 * @FilePath: /BoostPro1/master/server/src/urldispatch/url_router.hpp
 * @Description: url路由实现
 */
#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "baseurl_handler.hpp"

namespace chenglei {

class UrlRouter {
public:
    static UrlRouter& instance() {
        static UrlRouter inst;
        return inst;
    }

    void registerUrl(const std::string &url, std::unique_ptr<BaseUrlHandler> handler);

    BaseUrlHandler *get(const std::string &url);

private:
    UrlRouter() = default;
    UrlRouter(const UrlRouter&) = delete;
    UrlRouter& operator=(const UrlRouter& rt) = delete;

    std::unordered_map<std::string, std::unique_ptr<BaseUrlHandler>> handlers_;
    std::mutex mutex_;
};

}
#include "url_router.hpp"

namespace chenglei {

    void UrlRouter::registerUrl(const std::string& url, std::unique_ptr<BaseUrlHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_[url] = std::move(handler);
    }

    BaseUrlHandler* UrlRouter::get(const std::string& url) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = handlers_.find(url);
        if (it != handlers_.end()) return it->second.get();
        return nullptr;
    }
}
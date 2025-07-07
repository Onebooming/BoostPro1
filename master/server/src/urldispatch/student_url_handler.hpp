#ifndef _STUDENT_URL_HANDLER_HPP_
#define _STUDENT_URL_HANDLER_HPP_

#include "baseurl_handler.hpp"
#include "../controller/student_controller.hpp"
namespace http = boost::beast::http;

namespace chenglei {

class StudentUrlHandler : public BaseUrlHandler {
public:
    int url_handler(http::request<http::string_body> &request, http::response<http::string_body> &response) override;
};


}

#endif //_STUDENT_URL_HANDLER_HPP_
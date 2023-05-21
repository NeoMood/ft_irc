#include "../includes/Request.hpp"

Request::Request(): _cmd(),_message(), _params(), _keys(), _invalid_request(false) {}

std::string Request::getCmd() const {
    return this->_cmd;
}

std::vector<std::string> Request::getParams() const {
    return this->_params;
}

std::vector<std::string> Request::getKeys() const {
    return this->_keys;
}

void Request::parseRequest(std::string line) {
    size_t pos = line.find(" ");
    if (pos != std::string::npos) {
        // TODO: parse the line and build the request
    } else {
        this->_cmd = "";
        this->_invalid_request = true;
    }
}

Request::~Request() {
    _params.clear();
}
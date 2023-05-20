#include "../includes/Params.hpp"

Params::Params(): _cmd(), _params(), _keys() {}

std::string Params::getCmd() const {
    return this->_cmd;
}

std::vector<std::string> Params::getParams() const {
    return this->_params;
}

std::vector<std::string> Params::getKeys() const {
    return this->_keys;
}

void Params::parseParam(std::string line) {
    size_t pos = line.find(" ");
    if (pos != std::string::npos) {
        this->_cmd = line.substr(0, pos);
        line
    } else {
        this->_cmd = "";
    }
}

Params::~Params() {
    _params.clear();
}
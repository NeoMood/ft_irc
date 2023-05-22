#include "../includes/Request.hpp"

Args::Args(std::string arg, std::string key): _arg(arg), _key(key) {}

std::string Args::getArg() const {
    return this->_arg;
}

std::string Args::getKey() const {
    return this->_key;
}

Args::~Args() {}


Request::Request(): _cmd(), _invalid_request(false) {}

std::string Request::getCmd() const {
    return this->_cmd;
}

std::vector<std::string> Request::getRequest() const {
    return this->request;
}

std::vector<std::string>	Request::_split(std::string s, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> _v;

	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		_v.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	// if (s.find(";") != std::string::npos)
	// 	s.pop_back();
	_v.push_back(s);
	return _v;
}

void Request::parseRequest(std::string line) {
    size_t pos = line.find(" ");
    if (pos != std::string::npos) {
        // TODO: parse the line and build the request
        this->_cmd = line.substr(0, pos);
        line.erase(0, pos + 1);
        if (this->_cmd == "PASS") {
            request.push_back(line);
        } else if (this->_cmd == "NICK") {
            request.push_back(line);
        } else if (this->_cmd == "JOIN") {
            std::vector<std::string> v = _split(line, " ");
            for (size_t i = 0; i < v.size(); i++) {
                std::cout << "|" << v.at(i) << "|" << std::endl;
                std::size_t pos = v.at(i).find(",");
                if (pos != std::string::npos) {
                    request.push_back(v.at(i).substr(0, pos));
                } else {
                    request.push_back(v.at(i));
                }
            }
        } else if (this->_cmd == "MODE") {
            //
        } else if (this->_cmd == "PRIVMSG") {
            pos = line.find(" :");
            // if (pos != std::string::npos) {
            //     request.push_back(Args(line.substr(0, pos), ""));
            //     line.erase(0, pos + 2);
            //     request.push_back(Args(line, ""));
            // } else {
            //     this->_invalid_request = true;
            // }
        }
    } else {
        this->_cmd = line;
        this->_invalid_request = true;
    }
}

Request::~Request() {
    request.clear();
}
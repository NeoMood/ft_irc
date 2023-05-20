#pragma once

#include <string>

class Params {
    private:
        std::string _cmd;
        std::vector<std::string> _params;
        std::vector<std::string> _keys;
    public:
        Params();
        void parseParam(std::string line);
        std::string getCmd() const;
        std::vector<std::string> getParams() const;
        std::vector<std::string> getKeys() const;
        ~Params();
};
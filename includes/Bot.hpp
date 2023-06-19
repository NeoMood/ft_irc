#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define FILE_CORRUPTED -1

class Bot {
    private:
        std::string cliend_id;
        std::string secret_id;
        std::string token;
        time_t      created_date;
        static Bot* _instance;
        bool _valid;
        Bot();
        int prelogin();
        bool isExpired();
        int parseToken(std::string line);
        std::vector<std::string> getTimeOfNow();
    public:
        static Bot& getBot();
        std::string logtime(std::string login, std::string begin, std::string end);
        bool isValid() const;
        ~Bot();
};

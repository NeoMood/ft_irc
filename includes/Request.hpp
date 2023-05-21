#pragma once

#include <string>

class Request {
    private:
        std::string                 _cmd;
        std::string                 _message;
        std::vector<std::string>    _params;
        std::vector<std::string>    _keys;
        bool                        _invalid_request;
        std::vector<std::string>    _comma_separated_values(std::string line);
    public:
        Request();
        void parseRequest(std::string line);
        std::string getCmd() const;
        std::vector<std::string> getParams() const;
        std::vector<std::string> getKeys() const;
        ~Request();
};

// TYPES OF REQUEST: {
    // PASS <password> -> cmd = PASS, params = [<password>]
    // NICK <nickname> -> cmd = NICK, params = [<nickname>]
    // JOIN #channel1, &channel2 key1, key2 -> cmd = JOIN, params = [#channel1, &channel2], keys = [key1, key2]
    // PRIVMSG #channel :Hello -> cmd = PRIVMSG, params = [#channel], message = "Hello"
    // PRIVMSG ayoub :Hey -> cmd = PRIVMSG, params = [ayoub], message = "Hey"
// }
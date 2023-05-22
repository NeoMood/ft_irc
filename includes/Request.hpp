#pragma once

#include <string>
#include <iostream>

class Args {
    std::string _arg;
    std::string _key;
    public:
        Args(std::string arg, std::string key);
        std::string getArg() const;
        std::string getKey() const;
        ~Args();
};

class Request {
    private:
        std::string                 _cmd;
        // std::string                 _message;
        std::vector<std::string>         request; request.push_back("#madrid,", "&test", "key1", "key2")
        // std::vector<std::string>    _keys;
        bool                        _invalid_request;
        std::vector<std::string>    _comma_separated_values(std::string line);
    public:
        Request();
        void parseRequest(std::string line);
        std::string getCmd() const;
        std::vector<std::string> getRequest() const;
        std::vector<std::string> getKeys() const;
        std::vector<std::string>	_split(std::string s, std::string delimiter);
        ~Request();
};

// TYPES OF REQUEST: {
    // PASS <password> -> cmd = PASS, params = [<password>]
    // NICK <nickname> -> cmd = NICK, params = [<nickname>]
    // JOIN #channel1, &channel2 key1, key2 -> cmd = JOIN, params = [#channel1, &channel2], keys = [key1, key2]
    // PRIVMSG #channel :Hello -> cmd = PRIVMSG, params = [#channel, Hello]
    // PRIVMSG ayoub :Hey -> cmd = PRIVMSG, params = [ayoub, Hey]
    // MODE #madrid +o ayoub -> cmd = MODE, params = [#madrid, +o, ayoub], 
// }
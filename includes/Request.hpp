#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <utility>

class Request {
    private:
        std::string                 _cmd;
        std::vector<std::string>         request;
        std::vector<std::string>         channels;
        std::vector<std::string>         keys;
        std::vector<std::pair<std::string, std::string> > _request;
    public:
		// setters
        void setcmd(std::string _cmd);
		void setRequest_(std::vector<std::pair<std::string, std::string> > _vector);
        void parseRequest(std::string& line);
		void setChannel(std::vector<std::string> _vector);
		void setRequest(std::vector<std::string> _vector);
		void setKeys(std::vector<std::string> _vector);
		void join_strings(std::vector<std::string>& line);
		// getters
		std::vector<std::string>	getChannel(void) const;
		std::vector<std::string>	getRequest(void) const;
		std::vector<std::string>	getkeys(void) const;
		std::vector<std::pair<std::string, std::string> >	getRequest_(void) const;
        std::string getcmd(void) const;
		// general funs
        std::vector<std::string>	_split(std::string& line, std::string& delim);
		std::vector<std::pair<std::string, std::string> > _splitJOIN(std::string& line, std::string& delim);
        Request();
        ~Request();
};

// TYPES OF REQUEST: {
    // PASS <password> -> cmd = PASS, params = [<password>]
    // NICK <nickname> -> cmd = NICK, params = [<nickname>]
    // JOIN #channel1, &channel2 key1, key2 -> cmd = JOIN, params = [#channel1, &channel2], keys = [key1, key2]
    // PRIVMSG #channel :Hello -> cmd = PRIVMSG, params = [#channel, Hello]
    // MODE #madrid +o ayoub -> cmd = MODE, params = [#madrid, +o, ayoub]
	// KICK &Channel Yahya
	// INVITE yahya &channel
	// TOPIC #channel :upadate the topic
// }
#pragma once


#define RPL_CHANNEL(nick, name, host, chan) ":" + nick + "!" + name + "@" + host + " JOIN " + chan + "\r\n"
#define RPL_WELCOME(nick, user, host) ":irc_server 001 " + nick + ":Welcome to the irc_server Network, " + nick + "!" + user + "@" + host + "\n"
#define RPL_YOURHOST(nick, host) "irc_server 002 " + nick + " :Your host is " + host + "\n"
#define RPL_CREATED(date)       std::string("003 :This server was created ") + date
#define RPL_MYINFO              "004"

#define RPL_LUSERCLIENT         "251"
#define RPL_LUSEROP             "252"
#define RPL_LUSERUNKNOWN        "253"
#define RPL_LUSERCHANNELS       "254"
#define RPL_LUSERME             "255"

#define RPL_AWAY                "301"
#define RPL_UNAWAY              "305"
#define RPL_NOWAWAY             "306"

#define RPL_WHOISUSER           "311"
#define RPL_WHOISSERVER         "312"
#define RPL_WHOISOPERATOR       "313"
#define RPL_WHOISIDLE           "317"
#define RPL_ENDOFWHOIS          "318"
#define RPL_WHOISCHANNELS       "319"

#define RPL_WHOREPLY            "352"
#define RPL_ENDOFWHO            "315"

#define RPL_LIST                "322"
#define RPL_LISTEND             "323"

#define RPL_CHANNELMODEIS       "324"

#define RPL_NOTOPIC             "331"


#define RPL_MOTDSTART           "375"
#define RPL_MOTD                "372"
#define RPL_ENDOFMOTD           "376"

#define RPL_YOUREOPER           "381"

#define ERR_NOSUCHNICK          "401"
#define ERR_NOSUCHSERVER        "402"
#define ERR_NOSUCHCHANNEL(nick, chan)   "403 " + nick + " " + chan + " :No such channel\n"
#define ERR_CANNOTSENDTOCHAN    "404"
#define ERR_TOOMANYTARGETS      "407"
#define RPL_TOPIC(nick, chan) "332 " + nick + " " + chan + " :\r\n"
#define ERR_NORECIPIENT(cmd) std::string("411 :No recipient given ") + cmd + "\n"
#define RPL_NAMREPLY(nick, chan, users)  "353 " + nick + " = " + chan + " :@" + users + "\r\n"
#define RPL_ENDOFNAMES(chan) "366 " + chan + " :End of NAMES list\r\n"
#define ERR_NOTEXTTOSEND(void) "412 :No text to send\n"
#define ERR_UNKNOWNCOMMAND      "421"
#define ERR_NOMOTD              "422"
#define ERR_TOOMANYCHANNELS(nick, chan) "405 " + nick + " " + chan + " :You have joined too many channels\r\n"
#define ERR_NONICKNAMEGIVEN(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + " 431 " + nick + " :No nickname given\n"
#define ERR_ERRONEUSNICKNAME(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + " 432 " + nick + " :Erroneus nickname\n"
#define ERR_NICKNAMEINUSE(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + " 433 " + nick + " :Nickname is already in use\n"
#define ERR_UNAVAILRESOURCE     "437"
#define ERR_USERNOTINCHANNEL(nick, user, chan)    "441 " + nick + " " + user + " " + chan + " :They aren't on that channel\n"
#define ERR_NOTONCHANNEL        "442"
#define ERR_NOTREGISTERED(nick) "451" + nick + " :The client must be registred\n"
#define ERR_NEEDMOREPARAMS(cmd, nick)    "461 " + nick + " " + cmd + " :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + " 462 " + nick + " :You may not reregister\n"
#define ERR_PASSWDMISMATCH(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + " 464 " + nick + " :Password incorrect\n"
#define ERR_UNKNOWNMODE         "472"
#define ERR_INVITEONLYCHAN(nick, chan)      "473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n"
#define ERR_BANNEDFROMCHAN(nick, chan)  "474 " + nick + " " + chan + " :Cannot join channel (+b)\n"
#define ERR_CHANNELISFULL(nick, chan)       "471" + nick + " " + chan + " :Cannot join channel (+l)\r\n"
// #define ERR_BADCHANNELKEY       "475"
#define ERR_BADCHANMASK(nick, chan)     "476 " + nick + " " + chan + " :Bad Channel Mask\n"
#define ERR_NOPRIVILEGES        "481"
#define ERR_CHANOPRIVSNEEDED(nick, chan)    "482 " + nick + " " + chan + " :You're not channel operator\n"
#define ERR_UMODEUNKNOWNFLAG    "501"
#define ERR_USERSDONTMATCH      "502"
#define ERR_BADCHANNELKEY(nick, chan)         "475 " + nick + " " + chan + " :Cannot join channel (+k)\r\n"



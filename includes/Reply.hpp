#pragma once

// general reply: :hostname + " " + code + " " + nickname + " " + whatever
// privmsg, notice, wallops -> :nickname!username@hostname + " " + CMD + " " + params

#define RPL_CHANNEL(nick, name, host, chan) ":" + nick + "!" + name + "@" + host + " JOIN " + chan + "\r\n"

#define RPL_WELCOME(nick, user, host) "001 " + nick + ":Welcome to the irc_server Network, " + nick + "!" + user + "@" + host + "\r\n"
#define RPL_YOURHOST(nick, host) "002 " + nick + " :Your host is " + host + "\r\n"
#define RPL_CREATED(date)       std::string("003 :This server was created ") + date
#define RPL_MYINFO              "004"

#define RPL_LUSERCLIENT         "251"
#define RPL_LUSEROP             "252"
#define RPL_LUSERUNKNOWN        "253"
#define RPL_LUSERCHANNELS       "254"
#define RPL_LUSERME             "255"

#define RPL_AWAY                "301 <nick> :<away message>\r\n"
#define RPL_UNAWAY              "305"
#define RPL_NOWAWAY             "306"

#define RPL_WHOISUSER(nick, user, host, realname) "311 " + nick + " " + user + " " + host + " * " + realname + "\r\n"
#define RPL_WHOISSERVER(nick, user, host, info) "312 " + nick + " " + user + " " + host + " :" + info + " \r\n"
#define RPL_WHOISOPERATOR(nick, user) "313 " + nick + " " + user + " :is an IRC operator\r\n"
#define RPL_ENDOFWHO            "315"
#define RPL_WHOISIDLE(nick, user, idle) "317 " + nick + " " + user + " " + idle + " :seconds idle\r\n"
#define RPL_ENDOFWHOIS(nick, user) "318 " + nick + " " + user + " :End of WHOIS list\r\n"
#define RPL_WHOISCHANNELS(nick, user, chan) "319 " + nick + " " + user + " :" + chan + "\r\n"
#define RPL_LIST(chan, topic) "322 " + chan + " :" + topic + "\r\n"
#define RPL_LISTEND(nick) "323 " + nick + " :End of LIST\r\n"

#define RPL_WHOREPLY            "352"


#define RPL_CHANNELMODEIS(chan, nick, mode, option)       "324 :" + nick + " MODE " + chan + " " + mode + " :" + option + "\r\n"
#define RPL_NOTOPIC(nick, chan) "331 " + nick + " " + chan + " :\r\n"
#define RPL_TOPIC(nick, chan) "332 " + nick + " " + chan + " :\r\n"
#define RPL_INVITING(chan, nick) "341 " + chan + " " + nick + "Client is invired to the channel succesfully :\r\n"

#define RPL_NAMREPLY(nick, chan, users)  "353 " + nick + " = " + chan + " :@" + users + "\r\n"
#define RPL_ENDOFNAMES(chan) "366 " + chan + " :End of NAMES list\r\n"

#define RPL_MOTDSTART           "375"
#define RPL_MOTD                "372"
#define RPL_ENDOFMOTD           "376"

#define RPL_YOUREOPER(nick) "381 " + nick + " :You are now an IRC operator\r\n"

#define ERR_NOSUCHNICK(nick, user)  "401 " + nick + " " + user + " :No such nick/channel\r\n"
#define ERR_NOSUCHSERVER        "402"
#define ERR_NOSUCHCHANNEL(chan)   "403 " + chan + " :No such channel\r\n"
#define ERR_CANNOTSENDTOCHAN(nick, chan)    "404 " + nick + " " + chan + " :Cannot send to channel\r\n"
#define ERR_TOOMANYCHANNELS(nick, chan) "405 " + nick + " " + chan + " :You have joined too many channels\r\n"
#define ERR_TOOMANYTARGETS(targets)      "407 " + targets + " :recipients. No message delivered\r\n"
#define ERR_NORECIPIENT(cmd) "411 :No recipient given " + cmd + "\r\n"
#define ERR_NOTEXTTOSEND(void) "412 :No text to send\r\n"
#define ERR_UNKNOWNCOMMAND(cmd)      "421 " + cmd + " :Unknown command\r\n"
#define ERR_NOMOTD              "422"
#define ERR_NONICKNAMEGIVEN(nick) "431 " + nick + " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick, user, host) ":" + nick + "!" + user + "@" + host + " 432 " + nick + " :Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE(nick) "433 " + nick + " :Nickname is already in use\r\n"
#define ERR_UNAVAILRESOURCE     "437"
#define ERR_USERNOTINCHANNEL(nick, chan)    "441 " + nick + " " + chan + " :They aren't on that channel\r\n"
#define ERR_NOTONCHANNEL(chan)        "442 " + chan + " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(user, chan) "443 " + user + " " + chan + " :is already on channel\r\n"
#define ERR_NOTREGISTERED(nick) "451 " + nick + " :You have not registered\r\n"
#define ERR_NEEDMOREPARAMS(cmd, nick)    "461 " + nick + " " + cmd + " :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED(nick, user, host) ":" + nick + "!" + user + "@" + host + " 462 " + nick + " :You may not reregister\r\n"
#define ERR_PASSWDMISMATCH(nick, user, host) ":" + nick + "!" + user + "@" + host + " 464 " + nick + " :Password incorrect\r\n"
#define ERR_KEYSET(chan) "467 " + chan + " :Channel key already set\r\n"
#define ERR_CHANNELISFULL(nick, chan)       "471" + nick + " " + chan + " :Cannot join channel (+l)\r\n"
#define ERR_UNKNOWNMODE(mode, chan)         "472 " + mode + " :is unknown mode char to me for " + chan + "\r\n"
#define ERR_INVITEONLYCHAN(nick, chan)      "473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n"
#define ERR_BANNEDFROMCHAN(nick, chan)  "474 " + nick + " " + chan + " :Cannot join channel (+b)\r\n"
#define ERR_BADCHANNELKEY(nick, chan)         "475 " + nick + " " + chan + " :Cannot join channel (+k)\r\n"
#define ERR_BADCHANMASK(nick, chan)     "476 " + nick + " " + chan + " :Bad Channel Mask\r\n"
#define ERR_NOPRIVILEGES(nick)        "481 " + nick + " :Permission Denied- You're not an IRC operator\r\n"
#define ERR_CHANOPRIVSNEEDED(chan)    "482 " + chan + " :You're not channel operator\r\n"
#define ERR_UMODEUNKNOWNFLAG    "501"
#define ERR_USERSDONTMATCH      "502 :Cannot change mode for other users\r\n"

#define ERR_NOSUCHFILE(nick, filename) "991 " + nick + " " + filename + " :No such file\r\n"
#define RPL_FILENOTICE(nick, username, host, user, filename) ":" + nick + "!" + username + "@" + host + " NOTICE " + user + " :" + nick + " sent you a file :" + filename + "\r\n"
#define RPL_FILEDOWNLOADEDNOTICE(nick, username, host, filename, filen) ":" + nick + "!" + username + "@" + host + " NOTICE filename :" + filename + " has been download as " + filen + "\r\n"
#define ERR_INVALIDDATE(nick) "993 " + nick + " invalid date range\r\n"
#define RPL_LOGTIME(nick, reply) "994 " + nick + reply + "\r\n"
#define ERR_INVALIDKEYS(nick, reply) "995 " + nick + reply + "\r\n"
#define RPL_LISTFILE(nick, file) "996 " + nick + " :" + file + "\r\n"
#define RPL_LISTFILEEND(nick) "997 " + nick + " :End of LIST\r\n"

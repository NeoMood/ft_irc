/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:35:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/21 03:00:45 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cstdio>
#include <poll.h>
#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#include <map>

#pragma	once

#define ERR_PASSWDMISMATCH(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + " 464" + nick + " :Password incorrect\n"
#define ERR_NEEDMOREPARAMS(nick, cmd, user)   ":" + nick + "!" + user + "@" + "irc_server" + "461 " + nick + " " + cmd + " :Not enough parameters\n"
#define ERR_ALREADYREGISTRED(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + "462 " + nick + " :You may not reregister\n"
#define ERR_NONICKNAMEGIVEN(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + "431 " + nick + " :No nickname given\n"
#define ERR_ERRONEUSNICKNAME(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + "432 " + nick + " :Erroneus nickname\n"
#define ERR_NICKNAMEINUSE(nick, user) ":" + nick + "!" + user + "@" + "irc_server" + "433 " + nick + " :Nickname is already in use\n"
#define ERR_NORECIPIENT(cmd) std::string("411 :No recipient given ") + cmd + "\n"
#define ERR_NOTEXTTOSEND(void) "412 :No text to send\n"

#define RPL_WELCOME(nick, user, host) ":irc_server 001 " + nick + ":Welcome to the <network> Network, " + nick + "!" + user + "@" + host + "\n"
#define RPL_YOURHOST(nick, host) "irc_server 002 " + nick + " :Your host is " +  host + "\n"
class Client{
    private:
        bool    PasswordApproved;
        bool    NicknameSited;
        bool    UsernameSited;
        std::string Nickname;
        std::string	IPAddress;
        std::string UserName;
        std::string mode;
        std::string hostname;
        std::string RealName;
        int FdNumber;
    public:
        Client();
        ~Client();
    // Setters
        void	setNicknameSited(bool _Nicknamesited);
        void	setUsernameSited(bool _Usernamesited);
		void	setIPAddress(std::string _IPAdresss);
        void    setHostname(std::string _hostname);
        void    setRealName(std::string _realname);
		void	setPasswordApproved(bool _Password);
        void    setUserName(std::string _user);
        void	setfd_number(int fd_number);
        void    setMode(std::string _mode);
        void	setNickname(std::string Nickname);
    // Getters
        bool		getNickNameSited(void);
        bool		getUserNameSited(void);
        int			getFdNumber(void);
        bool		getPasswordApproved(void);
        std::string	getNickname(void);
        std::string	getHostname(void);
        std::string	getRealName(void);
        std::string	getUserName(void);
        std::string	getMode(void);
        
};
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayoubaqlzim <ayoubaqlzim@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:35:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/21 17:04:37 by ayoubaqlzim      ###   ########.fr       */
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

#define ERR_PASSWDMISMATCH(nick) "464 " + nick + " :Password incorrect\n"
#define ERR_NEEDMOREPARAMS(nick, cmd)   "461 " + nick + " " + cmd + " :Not enough parameters\n"
#define ERR_ALREADYREGISTRED(nick) "462 " + nick + " :You may not reregister\n"
#define ERR_NONICKNAMEGIVEN(nick) "431 " + nick + " :No nickname given\n"
#define ERR_ERRONEUSNICKNAME(nick) "432 " + nick + " :Erroneus nickname\n"
#define ERR_NICKNAMEINUSE(nick) "433 " + nick + " :Nickname is already in use\n"
#define ERR_NOTREGISTERED(nick) "451 " + nick + " :You have not registered\n"

// typedef enum {
//     a = 0,
//     i = 1,
//     w = 2,
//     r = 3,
//     o = 4,
//     O = 5,
//     s = 6,
// } modes_t;

// i: Set/remove Invite-only channel
// t: Set/remove the restrictions of the TOPIC command to channel
// operators
// k: Set/remove the channel key (password)
// o: Give/take channel operator privilege
// l: Set/remove the user limit to channel

typedef enum {
    i = 0,
    t = 1,
    k = 2,
    o = 3,
    l = 4,

} channel_mode_t;

//  The available modes are as follows:

//            a - user is flagged as away;
//            i - marks a users as invisible;
//            w - user receives wallops;
//            r - restricted user connection;
//            o - operator flag;
//            O - local operator flag;
//            s - marks a user for receipt of server notices.

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
        std::vector<channel_mode_t> channel_mode;
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
        void    setChannelMode(channel_mode_t _mode);
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
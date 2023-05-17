/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:35:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/17 22:23:04 by yamzil           ###   ########.fr       */
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

class Client{
    private:
        bool    isregistred;
        bool    nicknamesited;
        std::string Name;
        std::string ip_adress;
        int fd_number;
    public:
        Client(std::string Name, std::string ip_adress, int fd_number);
        Client();
        ~Client();
        void    setName(std::string Name);
        void    setip_adress(std::string ip_adress);
        void    setfd_number(int fd_number);
        std::string    getName(void);
        std::string	getip_adress(void);
        int	getfd_number(void);
        bool	getisregistred();
        bool    getnicknamesited();
        void    setnicknamesited(bool _nicknamesited);
		void	setisregistred(bool _isregistred);
};
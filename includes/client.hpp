/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:35:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/23 11:53:10 by yamzil           ###   ########.fr       */
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
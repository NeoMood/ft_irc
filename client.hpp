/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:35:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/15 22:03:40 by yamzil           ###   ########.fr       */
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
#include <stdexcept>
#include <map>

#pragma	once

class Client{
    private:
        bool    isregistred;
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
		void	setisregistred(bool _isregistred);
};
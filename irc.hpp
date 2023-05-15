/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 20:07:31 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/15 23:04:04 by yamzil           ###   ########.fr       */
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
#include "client.hpp"
#include <map>
#include <cstring>
#include <sstream>


#define BUFFER_SIZE 1024
#define	EXIT_FAILURE 1

#pragma	once

class irc_server{
	private:
		int	socket_fd;
		int	portno;
		std::string	passwd;
		int	accept_fd;
		int	poll_fds;
		std::vector<pollfd> vec_fd;
    public:
	    std::vector<std::string> full_command;
		std::map<int, Client>	guest;
		std::map<int, Client>	connected;
		void setSocketFd(int socket_fd);
		void setPassword(std::string passwd);
		int	getSocketFd(void);
		std::string	getPassword(void);
        irc_server();
        ~irc_server();
        void init_sockets(void);
		void bind_sockets(void);
		void listenToIncomingconnection(void);
		void AcceptToIncomingconnection(Client& Client_data);
		void send_message(int fd, std::string message);
		void multipleconnection(void);
		void check_port(char **argv);
		void createServer(void);
		void ReusableSocket(void);
		void non_blocking(void);
		// void recievingmessage(void);

	//  COMMAND FUNCTION
		void	PASS(std::vector<std::string>& full_command, Client &client);
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 21:12:21 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/17 22:23:13 by yamzil           ###   ########.fr       */
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
#include <map>
#include <stdexcept>
#include "client.hpp"
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
		std::map<int, Client>	connected;
		std::map<int, Client>	guest;
		std::set<std::string> nicknames;
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
		bool check_param(const char *nickname, Client &client);
		void createServer(void);
		void ReusableSocket(void);
		void non_blocking(void);
		// void recievingmessage(void);

	//  COMMAND FUNCTION
		void	PASS(std::string paramters, Client &client);
		void	NICK(std::string paramters, Client &client);
};

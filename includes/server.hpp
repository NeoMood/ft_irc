/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 21:12:21 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/23 11:22:22 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cstring>
#include <fcntl.h>
#include <cstdio>
#include <poll.h>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <stdexcept>
#include "Client.hpp"
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
		void multipleconnection(void);
		bool check_param(const char *nickname, Client &client);
		void ReusableSocket(void);
		void non_blocking(void);
	// UTILS FUNCTION
		void	welcome_message(int fd, std::string message);
		void	send_message(int fd, std::string message);
		void	check_port(char **argv);
		void	get_date(void);
	//  COMMAND FUNCTION
		void	PASS(std::string paramters, Client &client);
		void	NICK(std::string paramters, Client &client);
		void	USER(std::string parametrs, Client &client);
		void	PRIVMSG(std::string parametrs, Client &client);
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayoubaqlzim <ayoubaqlzim@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 21:12:21 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/22 17:28:12 by ayoubaqlzim      ###   ########.fr       */
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
#include <cstring>
#include <sstream>
#include "Logger.hpp"
#include "Channel.hpp"


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
		std::vector<Channel> channels;
		Logger& logger;
    public:
		class ChannelNotFound: public std::exception {
			public:
				virtual const char * what() const throw () {
      				return "Channel not found";
   				}
		};
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
		void	send_message(int fd, std::string message);
		void	welcome_message(int fd, Client& client);
		void	check_port(char **argv);
		void	get_date(void);
	//  COMMAND FUNCTION
		void	PASS(std::string paramters, Client &client); // yahya
		void	NICK(std::string paramters, Client &client); // yahya
		void	USER(std::string parametrs, Client &client); // yahya
		void	JOIN(std::string parametrs, Client& client); // ayoub
		void	PRIVMSG(std::string parametrs, Client& client); // ayoub send msg to channel, saad send message to user
		void	KICK(std::string parametrs, Client& client); // ayoub
		void	INVITE(std::string parametrs, Client& client); // saad 
		void	TOPIC(std::string parametrs, Client& client); // saad
		void	MODE(std::string parametrs, Client& client); // saad mode for user, ayoub mode for channel
		std::vector<Channel>::iterator findChannelByName(std::string channel);
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgmira <sgmira@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 21:12:21 by yamzil            #+#    #+#             */
/*   Updated: 2023/06/17 20:34:33 by sgmira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma	once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <algorithm>
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
#include "Logger.hpp"
#include "Channel.hpp"


#define BUFFER_SIZE 1024
#define	EXIT_FAILURE 1
#define MAX_CHANNELS 20


typedef enum {
	UNKOWN = -1,
	REMOVE = 0,
	ADD = 1,
} e_action;

class irc_server{
	private:
		int	socket_fd;
		int	portno;
		std::string msg;
		std::string host;
		std::string	passwd;
		int	accept_fd;
		int	poll_fds;
		std::vector<pollfd> vec_fd;
		std::vector<Channel> channels;
		Logger& logger;
		std::string server_pass;
		bool checkChannelMask(char c);
		e_action checkAction(std::string mode);
		void leaveAllChannels(Client& client);
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
		void AcceptIncomingconnection(Client& Client_data);
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
		void	JOIN(std::vector<std::pair<std::string, std::string> > _request, Client& client); // ayoub
		void	PASS(std::vector<std::string> request, Client &client); // yahya
		void	NICK(std::vector<std::string> request, Client &client); // yahya
		void	USER(std::vector<std::string> request, Client &client); // yahya
		void	PRIVMSG(std::vector<std::string> request, Client& client); // ayoub send msg to channel
		void	KICK(std::vector<std::string> request, Client& client); // ayoub
		void	INVITE(std::vector<std::string> request, Client& client); // saad
		void	TOPIC(std::vector<std::string> request, Client& client); // saad
		void	MODE(std::vector<std::string> request, Client& client); // saad mode for user, ayoub mode for channel
		void	OPER(std::vector<std::string> request, Client& client);
		void	NAMES(std::vector<std::string> request, Client& client);
		std::vector<Channel>::iterator findChannelByName(std::string channel);
		int is_present(std::vector<std::string> args, int index);
		bool isoperator(std::map<std::string, Client&> operators, int userFd);
		std::map<int, Client>::iterator findClient(std::string nickname);
};

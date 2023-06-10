/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayoubaqlzim <ayoubaqlzim@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 21:12:21 by yamzil            #+#    #+#             */
/*   Updated: 2023/06/10 21:13:48 by ayoubaqlzim      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


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
#include <cstring>
#include <sstream>
#include <fstream>
#include "Client.hpp"
#include "Logger.hpp"
#include "Channel.hpp"
#include "FileTransfer.hpp"
#include "Arg.hpp"
#include "Bot.hpp"
#pragma once

#define BUFFER_SIZE 1024
#define	EXIT_FAILURE 1
#define MAX_CHANNELS 2
#define DATABASE_PATH "./database"

#define E_UNKNOWN -1
#define E_REMOVE 0
#define E_ADD 1



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
		FileTransfer& filetransfer;
		Bot& spotnuts;
		std::string server_pass;
		std::map<std::string, File> _files;
		bool checkChannelMask(char c);
		std::vector<std::pair<int, Arg> > checkAction(std::string mode);
		bool checkMode(char c);
		std::vector<std::string> splitByDelm(std::string arg, std::string delm);
		std::vector<Channel> findChannelsUserBelongTo(Client& client);
		bool isLeap(int year) const;
		bool validateDate(std::string date, std::string dlm) const;
		bool validateValue(std::string value) const;
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
		void	welcome_message(int fd, std::string message);
		void	send_message(int fd, std::string message);
		void	check_port(char **argv);
		void	get_date(void);
	//  COMMAND FUNCTION
		void	JOIN(std::vector<std::pair<std::string, std::string> > _request, Client& client); // ayoub -> Done, left: handle error msgs
		void	PASS(std::vector<std::string> request, Client &client); // yahya
		void	NICK(std::vector<std::string> request, Client &client); // yahya
		void	USER(std::vector<std::string> request, Client &client); // yahya
		void	PRIVMSG(std::vector<std::string> request, Client& client); // ayoub send msg to channel and user -> Done, left: handle error msgs
		void	KICK(std::vector<std::pair<std::string, std::string> > _request, Client& client); // ayoub -> Done
		void	INVITE(std::vector<std::string> request, Client& client); // saad
		void	TOPIC(std::vector<std::string> request, Client& client); // saad
		void	MODE(std::vector<std::string> request, Client& client); // ayoub mode for channel -> done

		void	PART(std::vector<std::string> request, Client& client); // -> PART #ch1,&ch2 :Leaving bye, DONE
		void	NOTICE(std::vector<std::string> request, Client& client); // -> DONE
		void	QUIT(std::vector<std::string> request, Client& client); // -> QUIT :Gone to have lunch, ERROR :Closing Link: hostname (Quit: Gone to have lunch), in channels :nickname!username@hostname QUIT :Hey, DONE
		void	LIST(std::vector<std::string> request, Client& client); // -> LIST #ch1,&ch2 --> DONE
		void	NAMES(std::vector<std::string> request, Client& client); // -> NAMES &ch1,#ch2 --> DONE

		// =======================IN_PROGRESS========================================
		void	OPER(std::vector<std::string> request, Client& client); // -> DONE
		void	WALLOPS(std::vector<std::string> request, Client& client); // -> DONE
		void	WHOIS(std::vector<std::string> request, Client& client); // -> DONE
		void	SENDFILE(std::vector<std::string> request, Client& client); // -> Nearly done
		void	GETFILE(std::vector<std::string> request, Client& client); // -> Nearly done
		void	BOT(std::vector<std::string> request, Client& client); // in progress
		// ========================================================================
		std::vector<Channel>::iterator findChannelByName(std::string channel);
		std::map<int, Client>::iterator findClient(std::string nickname);
		void add_mode(Client& client, std::vector<Channel>::iterator& it, std::pair<int, Arg> pair, std::vector<std::string> request);
		void remove_mode(Client& client, std::vector<Channel>::iterator& it, std::pair<int, Arg> pair, std::vector<std::string> request);
		std::map<std::string, Client&>::iterator findUser(std::map<std::string, Client&>& users, std::string nickname);
		int is_present(std::vector<std::string> args, int index);
		std::string formatUserMessage(std::string nickname, std::string username, std::string hostname);
		std::string getHostAddress();
};

#pragma once

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
#include <cctype>
#include <map>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include "Client.hpp"
#include "Logger.hpp"
#include "Channel.hpp"
#include "FileTransfer.hpp"
#include "Arg.hpp"
#include "Bot.hpp"

#define BUFFER_SIZE 1024
#define	EXIT_FAILURE 1
#define MAX_CHANNELS 20
#define DATABASE_PATH "./database"

#define E_UNKNOWN -1
#define E_REMOVE 0
#define E_ADD 1



class irc_server{
	private:
		std::string host;
		std::string msg;
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
		bool isDir(const char* path);
		bool isFile(const char* path);
		bool checkChannelMask(char c);
		std::vector<std::pair<int, Arg> > checkAction(std::string mode);
		std::vector<std::pair<int, Arg> > checkUserAction(std::string mode);
		bool checkUserMode(char c);
		bool checkMode(char c);
		std::vector<std::string> splitByDelm(std::string arg, std::string delm);
		std::vector<Channel> findChannelsUserBelongTo(Client& client);
		bool isLeap(int year) const;
		bool validateDate(std::string date, std::string dlm) const;
		bool validateValue(std::string value) const;
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
		void AcceptToIncomingconnection(Client& Client_data);
		void multipleconnection(void);
		bool check_param(const char *nickname, Client &client);
		void ReusableSocket(void);
		void non_blocking(void);

		void	welcome_message(int fd, std::string message);
		void	send_message(int fd, std::string message);
		void	check_port(char **argv);
		void	get_date(Client& client);

		void	JOIN(std::vector<std::pair<std::string, std::string> > _request, Client& client);
		void	PASS(std::vector<std::string> request, Client &client);
		void	NICK(std::vector<std::string> request, Client &client);
		void	USER(std::vector<std::string> request, Client &client);
		void	PRIVMSG(std::vector<std::string> request, Client& client);
		void	KICK(std::vector<std::string> _request, Client& client);
		void	INVITE(std::vector<std::string> request, Client& client);
		void	TOPIC(std::vector<std::string> request, Client& client);
		void	MODE(std::vector<std::string> request, Client& client);

		void	PART(std::vector<std::string> request, Client& client);
		void	NOTICE(std::vector<std::string> request, Client& client);
		void	QUIT(std::vector<std::string> request, Client& client);
		void	LIST(std::vector<std::string> request, Client& client);
		void	NAMES(std::vector<std::string> request, Client& client);
		void	PONG(std::vector<std::string> request, Client& client);

		void	OPER(std::vector<std::string> request, Client& client);
		void	WALLOPS(std::vector<std::string> request, Client& client);
		void	WHOIS(std::vector<std::string> request, Client& client);
		void	SENDFILE(std::vector<std::string> request, Client& client);
		void	GETFILE(std::vector<std::string> request, Client& client);
		void	LISTFILE(std::vector<std::string> request, Client& client);
		void	BOT(std::vector<std::string> request, Client& client);
		std::vector<Channel>::iterator findChannelByName(std::string channel);
		std::map<int, Client>::iterator findClient(std::string nickname);
		void add_mode(Client& client, std::vector<Channel>::iterator& it, std::pair<int, Arg> pair, std::vector<std::string> request);
		void remove_mode(Client& client, std::vector<Channel>::iterator& it, std::pair<int, Arg> pair, std::vector<std::string> request);
		std::map<std::string, Client&>::iterator findUser(std::map<std::string, Client&>& users, std::string nickname);
		std::map<std::string, Client&>::iterator findOperator(std::map<std::string, Client&>& operators, std::string nickname);
		int is_present(std::vector<std::string> args, int index);
		std::string formatUserMessage(std::string nickname, std::string username, std::string hostname);
		std::string getHostAddress();
		void add_user_mode(Client& client, char mode);
		void remove_user_mode(Client& client, char mode);
		std::string buildMode(Client& client);
};

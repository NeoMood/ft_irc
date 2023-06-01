/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:46:26 by yamzil            #+#    #+#             */
/*   Updated: 2023/06/01 19:57:29 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Reply.hpp"
#include "../includes/Request.hpp"

void irc_server::init_sockets(void)
{
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		std::cerr << "Socket: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void irc_server::ReusableSocket(void)
{
	int flag = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag)) == -1)
	{
		std::cerr << "Set Socket: " << std::strerror(errno) << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

void irc_server::non_blocking(void)
{
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl: " << std::strerror(errno) << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

void irc_server::bind_sockets(void)
{
	struct sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(this->portno);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

void irc_server::listenToIncomingconnection()
{
	if (listen(socket_fd, SOMAXCONN) == -1)
	{
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

void irc_server::multipleconnection()
{
	Client Client_data;
	pollfd server_fd;

	server_fd.fd = socket_fd;
	server_fd.events = POLLIN;
	vec_fd.push_back(server_fd);
	while (true)
	{
		poll_fds = poll(&vec_fd[0], vec_fd.size(), -1);
		if (poll_fds == -1)
		{
			std::cerr << "poll: " << std::strerror(errno) << std::endl;
			close(socket_fd);
			exit(EXIT_FAILURE);
		}
		AcceptIncomingconnection(Client_data);
	}
}

void irc_server::AcceptIncomingconnection(Client &Client_data)
{
	char buffer[BUFFER_SIZE];
	for (size_t i = 0; i < vec_fd.size(); i++)
	{
		if (vec_fd[i].revents & POLLIN)
		{
			if (vec_fd[i].fd == this->socket_fd)
			{

				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
				if (accept_fd == -1)
				{
					std::cerr << "accept: " << std::strerror(errno) << std::endl;
					exit(EXIT_FAILURE);
				}
				Client_data.setfd_number(accept_fd);
				Client_data.setIPAddress(inet_ntoa(client_addr.sin_addr));
				guest[accept_fd] = Client_data;
				pollfd client_fd;
				client_fd.fd = accept_fd;
				client_fd.events = POLLIN;
				vec_fd.push_back(client_fd);
			}
			else
			{
				std::memset(&buffer, 0, sizeof(buffer));
				int nbytes = recv(vec_fd[i].fd, buffer, BUFFER_SIZE, 0);
				if (nbytes == -1)
				{
					if (nbytes == EAGAIN ||  nbytes == EWOULDBLOCK)
					{
						continue;
					}
					else
					{
						std::cerr << "recv: " << std::strerror(errno) << std::endl;
					}
				}
				else if (nbytes == 0)
				{
					close(vec_fd[i].fd);
					vec_fd.erase(vec_fd.begin() + i);
					std::cout << "See you later!" << std::endl;
				}
				else
				{
					Request object;
					std::string _message;
					std::string message(buffer);
					guest.at(vec_fd[i].fd).MessageFormat += message;

					size_t pos = guest.at(vec_fd[i].fd).MessageFormat.find_first_of("\r\n");
					while (pos != std::string::npos)
					{
						_message = guest.at(vec_fd[i].fd).MessageFormat.substr(0, pos);
						object.parseRequest(_message);
						if (!object.getcmd().compare(0, object.getcmd().length(), "PASS"))
							PASS(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "NICK"))
							NICK(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "USER"))
							USER(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "KICK"))
							KICK(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "JOIN"))
							JOIN(object.getRequest_(), guest[vec_fd[i].fd]);
						_message = guest.at(vec_fd[i].fd).MessageFormat.erase(0, pos + 2);
						pos = guest.at(vec_fd[i].fd).MessageFormat.find_first_of("\r\n");
					}
				}
			}
		}
	}
}

//// COMMANDS

void irc_server::PASS(std::vector<std::string> request, Client &client)
{
	if (!request.size())
	{
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "PASS", client.getUserName()));
		return;
	}
	else if (request[0] != this->passwd)
	{
		send_message(client.getFdNumber(), ERR_PASSWDMISMATCH(client.getNickname(), client.getUserName()));
		return;
	}
	else if (request[0] == this->passwd && !client.getPasswordApproved())
	{
		std::cout << "Password approved, you can set a nickname" << std::endl;
		client.setPasswordApproved(true);
		return;
	}
	else if (client.getPasswordApproved() == true)
	{
		send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname(), client.getUserName()));
		return;
	}
}

void irc_server::NICK(std::vector<std::string> request, Client &client)
{
	if (!request.size())
	{
		send_message(client.getFdNumber(), ERR_NONICKNAMEGIVEN(client.getNickname(), client.getUserName()));
	}
	else if (check_param(request[0].c_str(), client) && client.getPasswordApproved())
	{
		if (nicknames.find(request[0]) == nicknames.end())
		{
			if (client.getNickNameSited())
			{
				std::set<std::string>::iterator it = nicknames.begin();
				for (; it != nicknames.end(); ++it){
					if (nicknames.find(request[0]) == nicknames.end()){
						nicknames.erase(client.getNickname());
						nicknames.insert(request[0]);
						client.setNickname(request[0]);
						std::cout << "update your nickname to: " << client.getNickname() << std::endl;
						break;
					}
				}
			}
			else
			{
				client.setNickname(request[0]);
				std::cout << "Your Nickname is: " << client.getNickname() << std::endl;
				client.setNicknameSited(true);
				nicknames.insert(request[0]);
			}
		}
		else
		{
			send_message(client.getFdNumber(), ERR_NICKNAMEINUSE(request[0], client.getUserName()));
		}
	}
}

void irc_server::USER(std::vector<std::string> request, Client &client)
{

	if (request.size() != 4)
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "USER", client.getUserName()));
	else
	{
		if (client.getPasswordApproved() && client.getNickNameSited())
		{
			if (!client.getUserNameSited())
			{
				client.setUserName(request[0]);
				client.setMode(request[1]);
				client.setHostname(request[2]);
				client.setRealName(request[3]);
				client.setUsernameSited(true);
				welcome_message(client.getFdNumber(), RPL_WELCOME(client.getNickname(), client.getUserName(), client.getHostname()));
				welcome_message(client.getFdNumber(), RPL_YOURHOST(client.getNickname(), client.getHostname()));
				get_date();
			}
		}
		else
			send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname(), client.getUserName()));
	}
}

void irc_server::JOIN(std::vector<std::pair<std::string, std::string> > _request, Client &client)
{
	(void) _request;
	(void)client;
	// for (size_t i = 0; i < _request.size(); i++)
	// {
	// 	std::cout << _request[i].first << " : " << _request[i].second << std::endl;
	// 	std::cout << _request[i].first.length() << " : " << _request[i].second.length() << std::endl;
	// }
	// if (!_request.size()) {
	// 	send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN", client.getUserName()));
	// } else {
	// 	// Channel name must be started with '&', '#', '+' or '!' of length up to fifty (50) characters
	// 	// JOIN #test^Dtest
	// 	if (!client.getUserName().length()) {
	// 		send_message(client.getFdNumber(), ERR_NOTREGISTERED(client.getNickname()));
	// 		return ;
	// 	}
	// 	logger.log(INFO, parametrs);
	// 	if (parametrs[0] != '&' || parametrs[0] != '#' || parametrs[0] != '+' || parametrs[0] != '!'
	// 		|| parametrs.find(",") != std::string::npos) {
	// 		send_message(client.getFdNumber(), ERR_BADCHANMASK(client.getNickname(), parametrs));
	// 		return ;
	// 	}
	// 	if (!channels.size()) {
	// 		logger.log(DEBUG, client.getNickname());
	// 		channels.push_back(Channel(parametrs, client));
	// 	} else {
	// 		std::vector<Channel>::iterator it = findChannelByName(parametrs);

	// 		if (it != channels.end()) {
	// 			logger.log(DEBUG, "User request to join this channel");
	// 			if (!it->join_user(client)) {
	// 				send_message(client.getFdNumber(), ERR_BANNEDFROMCHAN(client.getNickname(), parametrs));
	// 				return ;
	// 			}
	// 		} else {
	// 			channels.push_back(Channel(parametrs, client));
	// 		}
	// 		it = findChannelByName(parametrs);
	// 		logger.log(DEBUG, "channel " + parametrs + " has "+ std::to_string(it->getUsers().size()) + " members");
	// 	}
	// }
}

// void irc_server::PRIVMSG(std::vector<std::string> request, Client& client) {
// 	(void) client;
// 	// if (parametrs[0] == '&' || parametrs[0] == '#' || parametrs[0] == '+' || parametrs[0] == '!') {
// 		// PRIVMSG #madrid hello world
// 	// logger.log(DEBUG, "Send messages to channel " + parametrs);
// 	for (size_t i = 0; i < channels.size(); i++) {
// 		std::map<std::string, Client&> users = channels[i].getUsers();
// 		for (std::map<std::string, Client&>::iterator it = users.begin(); it != users.end(); it++) {
// 			send_message(it->second.getFdNumber(), "Hello world\n");
// 		}
// 	}
// }

void irc_server::KICK(std::vector<std::string> request, Client &client)
{
	(void)client;
	for (size_t i = 0; i < request.size(); i++)
	{
		std::cout << request[i] << std::endl;
		std::cout << request[i].size() << std::endl;
	}
	// std::size_t pos = parametrs.find(" ");
	// if (pos != std::string::npos) {
	// 	std::string channelName = parametrs.substr(0, pos);
	// 	parametrs.erase(0, pos + 1);
	// 	// try {
	// 	std::vector<Channel>::iterator it_chan = findChannelByName(channelName);
	// 	if (it_chan != channels.end()) {
	// 		// channel.ban_user(client);
	// 		logger.log(DEBUG, "kick the user " + parametrs);
	// 		std::map<std::string, Client&> users = it_chan->getUsers();
	// 		std::map<std::string, Client&>::iterator it = users.end();
	// 		// logger.log(DEBUG, "User found " + std::to_string(it_chan->getUsers().size()));
	// 		for (std::map<std::string, Client&>::iterator t = users.begin(); t != users.end(); t++) {
	// 			if (t->first == parametrs) {
	// 				it = t;
	// 				break ;
	// 			}
	// 		}
	// 		if (it != users.end()) {
	// 			it_chan->ban_user(it->second);
	// 		} else {
	// 			send_message(client.getFdNumber(), ERR_USERNOTINCHANNEL(client.getNickname(), parametrs, channelName));
	// 			return ;
	// 		}

	// 	} else {
	// 		send_message(client.getFdNumber(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
	// 		return ;
	// 	}
	// 	// } catch(ChannelNotFound e) {
	// 	// }
	// 	// for (size_t i = 0; i < channels.size(); i++) {
	// 	// 	if (!channels[i].getChannelName().compare(0, parametrs.length(), parametrs)) {
	// 	// 		//
	// 	// 	}
	// 	// }
	// }
}

// void	irc_server::INVITE(std::vector<std::string> request, Client& client) {
// 	(void) client;
// 	logger.log(INFO, "invite command");
// }

// void	irc_server::TOPIC(std::vector<std::string> request, Client& client) {
// 	(void) client;
// 	logger.log(INFO, "topic command");
// }

// void	irc_server::MODE(std::vector<std::string> request, Client& client) {
// 	(void) client;
// 	logger.log(INFO, "mode command");
// }

/// GETTERS AND SETTERS

int irc_server::getSocketFd(void)
{
	return (this->socket_fd);
}

void irc_server::setSocketFd(int socket_fd)
{
	this->socket_fd = socket_fd;
}

void irc_server::setPassword(std::string passwd)
{
	this->passwd = passwd;
}

std::string irc_server::getPassword()
{
	return (this->passwd);
}

std::vector<Channel>::iterator irc_server::findChannelByName(std::string channel)
{
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->getChannelName() == channel)
		{
			return it;
		}
	}
	return channels.end();
}

irc_server::irc_server() : channels(), logger(Logger::getLogger())
{
}

irc_server::~irc_server()
{
}

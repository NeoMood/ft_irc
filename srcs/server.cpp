/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayoubaqlzim <ayoubaqlzim@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:46:26 by yamzil            #+#    #+#             */
/*   Updated: 2023/06/11 23:07:35 by ayoubaqlzim      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Reply.hpp"
#include "../includes/Request.hpp"
#include <algorithm>

void    irc_server::init_sockets(void){
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1){
        std::cerr << "Socket: " << std::strerror(errno) << std::endl;
        exit (EXIT_FAILURE);
    }
}

void	irc_server::ReusableSocket(void){
	int	flag = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag)) == -1){
		std::cerr << "Set Socket: " << std::strerror(errno) << std::endl;
		close (socket_fd);
        exit (EXIT_FAILURE);
	}
}

void	irc_server::non_blocking(void){
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1){
		std::cerr << "fcntl: " << std::strerror(errno) << std::endl;
		close (socket_fd);
        exit (EXIT_FAILURE);
	}
}

void	irc_server::bind_sockets(void){
	struct sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(this->portno);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		close (socket_fd);
        exit (EXIT_FAILURE);
	}
}

void	irc_server::listenToIncomingconnection(){
	if (listen(socket_fd, SOMAXCONN) == -1){
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		close (socket_fd);
        exit (EXIT_FAILURE);
	}
}

void	irc_server::multipleconnection(){
	Client	Client_data;
	pollfd	server_fd;

	server_fd.fd = socket_fd;
	server_fd.events = POLLIN;
	vec_fd.push_back(server_fd);
	while (true)
	{
		poll_fds = poll(&vec_fd[0], vec_fd.size(), -1);
		if ( poll_fds == -1){
			std::cerr << "poll: " << std::strerror(errno) << std::endl;
			close (socket_fd);
			exit (EXIT_FAILURE);
		}
		AcceptToIncomingconnection(Client_data);
	}
}

void	irc_server::AcceptToIncomingconnection(Client& Client_data){
	char buffer[BUFFER_SIZE];
	for (size_t i = 0; i < vec_fd.size(); i++) {
		if (vec_fd[i].revents & POLLIN){
			if (vec_fd[i].fd == this->socket_fd){
				
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
				if (accept_fd == -1){
					std::cerr << "accept: " << std::strerror(errno) << std::endl;
					exit (EXIT_FAILURE);
				}
				Client_data.setJoiningTime(time(0));
				Client_data.setfd_number(accept_fd);
				Client_data.setIPAddress(inet_ntoa(client_addr.sin_addr));
				guest.insert(std::pair<int, Client>(accept_fd, Client_data));
				pollfd client_fd;
				client_fd.fd = accept_fd;
				client_fd.events = POLLIN;
				vec_fd.push_back(client_fd);
			}
			else {
				std::memset(&buffer, 0, sizeof(buffer));
				int nbytes = recv(vec_fd[i].fd, buffer, BUFFER_SIZE, 0);
				if (nbytes == -1){
					std::cerr << "recv: " << std::strerror(errno) << std::endl;
				}
				else if (nbytes == 0){
					leaveAllChannels(guest[vec_fd[i].fd]);
					nicknames.erase(guest[vec_fd[i].fd].getNickname());
					guest.erase(vec_fd[i].fd);
					close(vec_fd[i].fd);
					vec_fd.erase(vec_fd.begin() + i);
					std::cout << "See you later!" << std::endl;
				}
				else {
					Request object;
					std::string _message;
					std::string message(buffer);
					guest.at(vec_fd[i].fd).MessageFormat += message;

					size_t pos = guest.at(vec_fd[i].fd).MessageFormat.find_first_of("\r\n");
					while (pos != std::string::npos)
					{
						_message = guest.at(vec_fd[i].fd).MessageFormat.substr(0, pos);
						logger.log(DEBUG, "Message " + std::to_string(_message.length()));
						object.parseRequest(_message);
						if (!object.getcmd().compare(0, object.getcmd().length(), "PASS"))
							PASS(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "NICK"))
							NICK(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "USER"))
							USER(object.getRequest(), guest[vec_fd[i].fd]);
						else {
							if (!guest[vec_fd[i].fd].getUserName().empty()) {
								if (!object.getcmd().compare(0, object.getcmd().length(), "KICK"))
									KICK(object.getRequest_(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "JOIN"))
									JOIN(object.getRequest_(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "PRIVMSG"))
									PRIVMSG(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "INVITE"))
									INVITE(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "TOPIC"))
									TOPIC(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "MODE"))
									MODE(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "SENDFILE"))
									SENDFILE(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "GETFILE"))
									GETFILE(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "LISTFILE"))
									LISTFILE(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "SPOT.NUTS"))
									BOT(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "PART"))
									PART(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "QUIT"))
									QUIT(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "LIST"))
									LIST(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "NAMES"))
									NAMES(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "NOTICE"))
									NOTICE(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "OPER"))
									OPER(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "WALLOPS"))
									WALLOPS(object.getRequest(), guest[vec_fd[i].fd]);
								else if (!object.getcmd().compare(0, object.getcmd().length(), "WHOIS"))
									WHOIS(object.getRequest(), guest[vec_fd[i].fd]);
								else
									send_message(vec_fd[i].fd, msg + ERR_UNKNOWNCOMMAND(_message));
							} else {
								send_message(vec_fd[i].fd, msg + ERR_NOTREGISTERED(guest[vec_fd[i].fd].getNickname()));
							}
						}
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
	if (!request.size()){
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(std::string("PASS"), client.getNickname()));
		return;
	}
	else if (request[0] != this->passwd){
		send_message(client.getFdNumber(), ERR_PASSWDMISMATCH(client.getNickname(), client.getUserName(), host));
		return;
	}
	else if (request[0] == this->passwd && !client.getPasswordApproved()){
		std::cout << "Password approved, you can set a nickname" << std::endl;
		client.setPasswordApproved(true);
		return ;
	}
	else if (client.getPasswordApproved() == true){
		send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname(), client.getUserName(), host));
		return ;
	}
}

void irc_server::NICK(std::vector<std::string> request, Client &client)
{
	if (!request.size()){
		send_message(client.getFdNumber(), ":" + getHostAddress() + " " + ERR_NONICKNAMEGIVEN(client.getNickname()));
	}
	else if (check_param(request[0].c_str(), client) && client.getPasswordApproved()){
		if (nicknames.find(request[0]) == nicknames.end()){
			client.setNickname(request[0]);
			std::cout << "Your Nickname is: " << client.getNickname() << std::endl;
			client.setNicknameSited(true);
			nicknames.insert(request[0]);
		}
		else{
			send_message(client.getFdNumber(), ":" + getHostAddress() + " " + ERR_NICKNAMEINUSE(request[0]));
		}
	}
}

void irc_server::USER(std::vector<std::string> request, Client &client){
	if (!request.size())
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("USER"), client.getNickname()));
	else{
		if (client.getPasswordApproved() && client.getNickNameSited()){
			if (!client.getUserNameSited()){
				client.setUserName(request[0]);
				client.setMode(request[1]);
				client.setHostname(request[2]);
				client.setRealName(request[3]);
				client.setUsernameSited(true);
				welcome_message(client.getFdNumber(), msg + RPL_WELCOME(client.getNickname(), client.getUserName(), host));
				welcome_message(client.getFdNumber(), msg + RPL_YOURHOST(client.getNickname(), host));
				get_date();
			}
		}
		else
			send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname(), client.getUserName(), host));
	}
}

bool irc_server::checkChannelMask(char c) {
	return (c != '#' && c != '!' && c != '+' && c != '&');
}

// SENDFILE ayoub :/path/to/file
// SENDFILE #madrid :/path/to/file
// GETFILE ayoub name
// BOT

// :nickname!username@hostname JOIN channelName
// :hostname 332 lab #madrid :
// :hostname 353 lab = #madrid :@ayoub omar lab 
// :hostname 356 lab #madrid :End of NAMES list

void irc_server::JOIN(std::vector<std::pair<std::string, std::string> > _request, Client& client) {
	//   Numeric Replies:

    //        ERR_NEEDMOREPARAMS: [D]              ERR_BANNEDFROMCHAN: [D]
    //        ERR_INVITEONLYCHAN: [D]              ERR_BADCHANNELKEY: [D]
    //        ERR_CHANNELISFULL: [D]               ERR_BADCHANMASK: [D]
    //        ERR_NOSUCHCHANNEL: [N]               ERR_TOOMANYCHANNELS: [D]
    //        ERR_TOOMANYTARGETS: [N]              ERR_UNAVAILRESOURCE: [N]
    //        RPL_TOPIC: [D]
	if (_request.empty()) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("JOIN"), client.getNickname()));
		return ;
	}
	// if (!client.getUserName().length()) {
	// 	send_message(client.getFdNumber(), msg + ERR_NOTREGISTERED(client.getNickname()));
	// 	return ;
	// }
	if (_request.size() == 1 && _request[0].first == "0") {
		leaveAllChannels(client);
		client.decrementChannelCount();
		return ;
	}
	for (size_t i = 0; i < _request.size(); i++) {
		std::pair<std::string, std::string> pair = _request[i];
		if (checkChannelMask(pair.first[0]) || pair.first.find(",") != std::string::npos || pair.first.find(" ") != std::string::npos) {
			send_message(client.getFdNumber(), msg + ERR_BADCHANMASK(client.getNickname(), pair.first));
		} else {
			std::vector<Channel>::iterator it = findChannelByName(pair.first);
			if (it != channels.end()) {
				if (client.getChannelCount() >= MAX_CHANNELS) {
					send_message(client.getFdNumber(), msg + ERR_TOOMANYCHANNELS(client.getNickname(), pair.first));
					return ;
				}
				logger.log(DEBUG, "User request to join channel: |" + pair.first + "|");
				if (!it->is_already_join(client) &&  it->getUserLimit() != -1 && it->getUsersTotal() >= it->getUserLimit()) {
					send_message(client.getFdNumber(), msg + ERR_CHANNELISFULL(client.getNickname(), pair.first));
					return ;
				}
				if (it->hasKey()) {
					logger.log(DEBUG, "channel has a key");
					if (!it->getChannelKey().compare(0, it->getChannelKey().length(), pair.second)) {
						logger.log(DEBUG, "Channel key is correct");
						if (!it->join_user(msg, host, client)) {
							send_message(client.getFdNumber(), msg + ERR_BANNEDFROMCHAN(client.getNickname(), pair.first));
							return ;
						}
					} else {
						logger.log(DEBUG, "channel key is not valid");
						send_message(client.getFdNumber(), msg + ERR_BADCHANNELKEY(client.getNickname(), pair.first));
						return ;
					}
				} else {
					logger.log(DEBUG, "channel has no key hh");
					if (!it->join_user(msg, host, client)) {
						send_message(client.getFdNumber(), msg + ERR_BANNEDFROMCHAN(client.getNickname(), pair.first));
						return ;
					}
				}
			} else {
				logger.log(DEBUG, "Channel: " + pair.first + " Not found, will create one");
				channels.push_back(Channel(pair.first, client));
			}
		}
	}
}

void irc_server::PRIVMSG(std::vector<std::string> request, Client& client) {
	//  Numeric Replies:

    //        ERR_NORECIPIENT: [D]                 ERR_NOTEXTTOSEND: [D] --> privmsg #mad
    //        ERR_CANNOTSENDTOCHAN: [D]            ERR_NOTOPLEVEL: [X]
    //        ERR_WILDTOPLEVEL: [X]                ERR_TOOMANYTARGETS: [D] --> privmsg ayoub,omar :hey -> split users with , not space
    //        ERR_NOSUCHNICK: [D]
    //        RPL_AWAY: [N] -> check if user is away
	bool too_may_args = false;
	std::string targets;
	
	for (size_t i = 0; i < request.size(); i++) {
		std::size_t pos = request[i].find(",");
		while (pos != std::string::npos) {
			too_may_args = true;
			targets += (request[i].substr(0, pos) + " ");
			request[i].erase(0, pos + 1);
			pos = request[i].find(",");
		}
		if (request[i].find(":") == std::string::npos) {
			targets += request[i].substr(0, request[i].find(" "));
		}
	}
	if (too_may_args) {
		send_message(client.getFdNumber(), msg + ERR_TOOMANYTARGETS(targets));
		return ;
	}

	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NORECIPIENT(std::string("PRIVMSG")));
		return ;
	} else if (!is_present(request, 1) || request[1].empty()) {
		send_message(client.getFdNumber(), msg + ERR_NOTEXTTOSEND());
		return ;
	}
	if (!checkChannelMask(request[0][0])) {
		std::vector<Channel>::iterator it = findChannelByName(request[0]);
		logger.log(DEBUG, "Send msg: " + request[1] + " To channel: |" + request[0] + "|");
		if (it != channels.end()) {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&>::iterator u = findUser(users, client.getNickname());
			if (client.getNickname() != it->getCreator().getNickname() && u == users.end()) {
				send_message(client.getFdNumber(), msg + ERR_CANNOTSENDTOCHAN(client.getNickname(), request[0]));
				return ;
			}
			std::cout << "users size: " << users.size() << std::endl;
			if (client.getFdNumber() != it->getCreator().getFdNumber()) {
				send_message(it->getCreator().getFdNumber(), formatUserMessage(client.getNickname(),
						client.getUserName(), host) + "PRIVMSG " + request[0] + " " + request[1] + "\r\n");
			}
			for (std::map<std::string, Client&>::iterator it = users.begin(); it != users.end(); it++) {
				if (client.getFdNumber() != it->second.getFdNumber()) {
					send_message(it->second.getFdNumber(), formatUserMessage(client.getNickname(),
						client.getUserName(), host) + "PRIVMSG " + request[0] + " " + request[1] + "\r\n");
				}
			}
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(request[0]));
		}
	} else {
		std::map<int, Client>::iterator it = findClient(request[0]);
		if (it != guest.end()) {
			send_message(it->first, formatUserMessage(client.getNickname(),
				client.getUserName(), host) + "PRIVMSG " + it->second.getNickname() + " " + request[1] + "\r\n");
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(client.getNickname(), request[0]));
		}
	}
}


void	irc_server::KICK(std::vector<std::pair<std::string, std::string> > _request, Client& client) {
	// Numeric Replies:

    //        ERR_NEEDMOREPARAMS: [D]              ERR_NOSUCHCHANNEL: [D]
    //        ERR_BADCHANMASK: [D]                 ERR_CHANOPRIVSNEEDED: [D]
    //        ERR_USERNOTINCHANNEL: [D]            ERR_NOTONCHANNEL: [D]
	logger.log(INFO, "kick command for channel");
	std::cout << "Size: " << _request.size() << std::endl;
	for (size_t i = 0; i < _request.size(); i++) {
		std::pair<std::string, std::string> pair = _request[i];
		std::cout << "Channel name: " << pair.first << " user: " << pair.second << std::endl;
		if (!client.getUserName().length()) {
			send_message(client.getFdNumber(), msg + ERR_NOTREGISTERED(client.getNickname()));
			return ;
		}
		if (checkChannelMask(pair.first[0]) || pair.first.find(",") != std::string::npos
			|| pair.first.find(" ") != std::string::npos) {
			send_message(client.getFdNumber(), msg + ERR_BADCHANMASK(client.getNickname(), pair.first));
		} else {
			if (!pair.first.length() || !pair.second.length()) {
				send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("KICK"), client.getNickname()));
			}
			std::vector<Channel>::iterator it = findChannelByName(pair.first);
			if (it != channels.end()) {
				if (client.getNickname() == pair.second) {
					return ;
				}
				if (it->hasUser(client.getNickname())) {
					if (it->isAnOperatorOrOwner(client)) {
						std::map<std::string, Client&> users = it->getUsers();
						std::map<std::string, Client&>::iterator u = findUser(users, pair.second);
						if (u != users.end()) {
							logger.log(DEBUG, "Will kick this user");
							it->remove_user(u->second);
						} else {
							send_message(client.getFdNumber(), msg + ERR_USERNOTINCHANNEL(client.getNickname(), pair.first));
						}
					} else {
						send_message(client.getFdNumber(), msg + ERR_CHANOPRIVSNEEDED(pair.first));
					}
				} else {
					send_message(client.getFdNumber(), msg + ERR_NOTONCHANNEL(pair.first));
				}
			} else {
				send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(pair.first));
			}
		}
	}
}

void	irc_server::INVITE(std::vector<std::string> request, Client& client) {
	// Replies
	// ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
	//    ERR_NOTONCHANNEL                ERR_USERONCHANNEL
	//    ERR_CHANOPRIVSNEEDED
	//    RPL_INVITING                    RPL_AWAY
	logger.log(DEBUG, "invite command");
	std::vector<Channel>::iterator it = findChannelByName(request[0]);
	if (it != channels.end()) {
		logger.log(DEBUG, "Channel found " + request[0]);
		if (it->isAnOperatorOrOwner(client)) {
			std::map<int, Client>::iterator u = findClient(request[1]);
			if (u != guest.end()) {
				if (u->first != client.getFdNumber()) {
					it->invite_user(u->second);
				} else {
					send_message(client.getFdNumber(), "Can not invite your self\n");
				}
			} else {
				send_message(client.getFdNumber(), ERR_NOSUCHNICK(request[1], request[0]));
			}
		} else {
			logger.log(DEBUG, "Action can not be permited");
			return ;
		}
	} else {
		logger.log(DEBUG, "Channel not found");
		return ;
	}
}

void	irc_server::TOPIC(std::vector<std::string> request, Client& client) {
	(void) request;
	(void) client;
	logger.log(DEBUG, "topic command");
}
// curl --upload-file ./txt https://free.keep.sh
// output: https://free.keep.sh/Y8v3dw9XxotVpTgV/txt

void	irc_server::SENDFILE(std::vector<std::string> request, Client& client) {
	// ERR_NEEDMOREPARAMS: [D]
	// ERR_NOSUCHNICK: [D], ERR_NOSUCHFILE: [D]
	// sendfile username pathtofile
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("SENDFILE"), client.getNickname()));
	}
	std::vector<std::string> args = splitByDelm(request[0], " ");
	if (!is_present(args, 0) || !is_present(args, 1)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("SENDFILE"), client.getNickname()));
		return ;
	}
	std::map<int, Client>::iterator it = findClient(args[0]);
	if (it != guest.end()) {
		if (isDir(args[1].c_str())) {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHFILE(client.getNickname(), args[1]));
			return ;
		}
		if (isFile(args[1].c_str())) {
			filetransfer.uploadFile(client.getNickname(), args[0], args[1], _files);
			send_message(it->first, RPL_FILENOTICE(client.getNickname(), client.getUserName(),
					getHostAddress(), it->second.getNickname(), args[1]));
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHFILE(client.getNickname(), args[1]));
		}
	} else {
		send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(client.getNickname(), args[0]));
	}
}
void	irc_server::GETFILE(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("GETFILE"), client.getNickname()));
	}
	std::vector<std::string> args = splitByDelm(request[0], " ");
	if (!is_present(args, 0) || !is_present(args, 1)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("GETFILE"), client.getNickname()));
		return ;
	}
	std::map<int, Client>::iterator it = findClient(args[0]);
	if (it != guest.end()) {
		std::string filename = filetransfer.downloadFile(args[1], args[0], _files);
		if (filename == "") {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHFILE(client.getNickname(), args[1]));
		} else {
			send_message(client.getFdNumber(), RPL_FILEDOWNLOADEDNOTICE(client.getNickname(), client.getUserName(), getHostAddress(), args[1], filename));
		}
	} else {
		send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(client.getNickname(), args[0]));
	}
}

void	irc_server::LISTFILE(std::vector<std::string> request, Client& client) {
	std::string reply;
	if (is_present(request, 0)) {
		reply += filetransfer.listFiles(msg, request[0], client.getNickname(), _files);
	} else {
		reply += filetransfer.listFiles(msg, "", client.getNickname(), _files);
	}
	send_message(client.getFdNumber(), reply);
}

void	irc_server::BOT(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("SPOT.NUTS"), client.getNickname()));
		return ;
	}
	if (!spotnuts.isValid()) {
		send_message(client.getFdNumber(), msg + ERR_INVALIDKEYS(client.getNickname(), " missing CLIENT_ID, SECRET_ID env"));
		return ;
	}
	std::vector<std::string> args = splitByDelm(request[0], " ");
	if ((is_present(args, 1) && !validateDate(args[1], "-")) || (is_present(args, 2) && !validateDate(args[2], "-"))) {
		send_message(client.getFdNumber(), msg + ERR_INVALIDDATE(client.getNickname()));
		return ;
	}
	std::string lgtime = spotnuts.logtime(args[0], is_present(args, 1) ? args[1] : "", is_present(args, 2) ? args[2] : "") + "h";
	send_message(client.getFdNumber(), msg + RPL_LOGTIME(client.getNickname(), " logtime for " + args[0] + " :" + lgtime));
}

void	irc_server::MODE(std::vector<std::string> request, Client& client) {
	logger.log(DEBUG, "mode command");
	if (request[0][0] == '#' || request[0][0] == '+' || request[0][0] == '!' || request[0][0] == '&') {
		logger.log(DEBUG, "Basic implementation of mode operations on channels");
		// channel mode
		// . i: Set/remove Invite-only channel
		// · t: Set/remove the restrictions of the TOPIC command to channel
		// operators
		// · k: Set/remove the channel key (password)
		// · o: Give/take channel operator privilege
		// . l: Set/remove the user limit to channel
		std::vector<Channel>::iterator it = findChannelByName(request[0]);
		if (it != channels.end()) {
		// 	Numeric Replies:
		
		// Errors
        //    ERR_NEEDMOREPARAMS: [D]              ERR_KEYSET: [D]
        //    ERR_NOCHANMODES: [N]                 ERR_CHANOPRIVSNEEDED: [D]
        //    ERR_USERNOTINCHANNEL: [D]            ERR_UNKNOWNMODE: [D]
		// Replies
        //    RPL_CHANNELMODEIS
        //    RPL_BANLIST                     RPL_ENDOFBANLIST
        //    RPL_EXCEPTLIST                  RPL_ENDOFEXCEPTLIST
        //    RPL_INVITELIST                  RPL_ENDOFINVITELIST
        //    RPL_UNIQOPIS
			logger.log(DEBUG, "Channel found " + request[0]);
			if (it->isAnOperatorOrOwner(client)) {
				logger.log(DEBUG, "user is an actual owner or operator of the channel");
				std::string mode = request[1];
				std::vector<std::pair<int, Arg> > actions = checkAction(mode);
				request.erase(request.begin(), request.begin() + 2);
				for (size_t i = 0; i < actions.size(); i++) {
					std::pair<int, Arg> pair = actions[i];
					if (pair.first == E_UNKNOWN) {
						logger.log(DEBUG, "Unkown mode prefix ");
						send_message(client.getFdNumber(), msg + ERR_UNKNOWNMODE(std::string(1, pair.second.getChar()), it->getChannelName()));
					} else if (pair.first == E_ADD) {
						add_mode(client, it, pair, request);
						std::cout << "action: "<< pair.first << " mode: " << pair.second.getChar() << " index: " << pair.second.getIndex() << std::endl;
					} else {
						remove_mode(client, it, pair, request);
						std::cout << "action: "<< pair.first << " mode: " << pair.second.getChar() << " index: " << pair.second.getIndex() << std::endl;
					}
				}
			} else {
				logger.log(DEBUG, "You are not an owner or an operator");
				send_message(client.getFdNumber(), msg + ERR_CHANOPRIVSNEEDED(it->getChannelName()));
			}
		}
		else {
			logger.log(DEBUG, "Channel not found");
			send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(request[0]));
			return ;
		}
	}
	else {
		// USER MODE
	}
}

void irc_server::add_mode(Client& client, std::vector<Channel>::iterator& it, std::pair<int, Arg> pair, std::vector<std::string> request) {
	logger.log(DEBUG, "mode prefix ADD");
	int index = pair.second.getIndex();
	if (pair.second.getChar() == 'i') {
		it->setInviteOnly(true);
		it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
			client.getNickname(), std::string("+i"), std::string("")), true);
	}
	if (pair.second.getChar() == 't') {
		it->setSupportTopic(true);
		it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
			client.getNickname(), std::string("+t"), std::string("")), true);
	}
	if (pair.second.getChar() == 'k') {
		if (!is_present(request, index)) {
			logger.log(DEBUG, "Error here");
			send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("MODE"), client.getNickname()));
		} else {
			if (it->getChannelKey() != "")
				send_message(client.getFdNumber(), msg + ERR_KEYSET(it->getChannelName()));
			else {
				it->setChannelKey(request[index]);
				it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
					client.getNickname(), std::string("+k"), request[index]), true);
			}
		}
	}
	if (pair.second.getChar() == 'o') {
		if (!is_present(request, index)) {
			logger.log(DEBUG, "Error here");
			send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("MODE"), client.getNickname()));
		} else {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&>::iterator u = findUser(users, request[index]);
			if (u != users.end()) {
				if (it->hasUser(request[index])) {
						it->add_operator(u->second);
						it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
							client.getNickname(), std::string("+o"), request[index]), true);
				} else {
					logger.log(DEBUG, "User not found in this channel");
					send_message(client.getFdNumber(), msg + ERR_USERNOTINCHANNEL(request[index], it->getChannelName()));
				}
			} else {
				logger.log(DEBUG, "user not found");
				send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(request[index], it->getChannelName()));
			}
		}
	}
	if (pair.second.getChar() == 'l') {
		if (!is_present(request, index)) {
			logger.log(DEBUG, "Error here");
			send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("MODE"), client.getNickname()));
		} else {
			it->setChannelLimit(std::atoi(request[index].c_str()));
			it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
				client.getNickname(), std::string("+l"), request[index]), true);
		}
	}
}

void irc_server::remove_mode(Client& client, std::vector<Channel>::iterator& it, std::pair<int, Arg> pair, std::vector<std::string> request) {
	logger.log(DEBUG, "mode prefix REMOVE");
	int index = pair.second.getIndex();
	if (pair.second.getChar() == 'i') {
		it->setInviteOnly(false);
		it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
			client.getNickname(), std::string("-i"), std::string("")), true);
	}
	if (pair.second.getChar() == 't') {
		it->setSupportTopic(false);
		it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
			client.getNickname(), std::string("-t"), std::string("")), true);
	}
	if (pair.second.getChar() == 'k') {
		if (!is_present(request, index)) {
			logger.log(DEBUG, "Error here");
			send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("MODE"), client.getNickname()));
		} else if (it->getChannelKey().compare(0, it->getChannelKey().length(), request[index])) {
			logger.log(ERROR, "unmatched keys");
		} else {
			it->setChannelKey("");
			it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
				client.getNickname(), std::string("-k"), request[index]), true);
		}
	}
	if (pair.second.getChar() == 'o') {
		if (!is_present(request, index)) {
			logger.log(DEBUG, "Error here");
			send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("MODE"), client.getNickname()));
		} else {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&>::iterator u = findUser(users, request[index]);
			if (u != users.end()) {
				if (it->hasUser(request[index])) {
						it->remove_operator(u->second);
						it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
							client.getNickname(), std::string("-o"), request[index]), true);
				} else {
					logger.log(DEBUG, "User not found in this channel");
				}
			} else {
				logger.log(DEBUG, "user not found");
			}
		}
	}
	if (pair.second.getChar() == 'l') {
		it->setChannelLimit(-1);
		it->sendToAllUsers(client, msg + RPL_CHANNELMODEIS(it->getChannelName(),
			client.getNickname(), std::string("-l"), std::string("")), true);
	}
}

std::vector<std::string> irc_server::splitByDelm(std::string arg, std::string delm) {
	std::vector<std::string> args;
	std::size_t pos = arg.find(delm);
	while (pos != std::string::npos) {
		args.push_back(arg.substr(0, pos));
		arg.erase(0, pos + delm.length());
		pos = arg.find(delm);
	}
	args.push_back(arg);
	return args;
}

void irc_server::PART(std::vector<std::string> request, Client& client) {
	if (!request.size()) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("PART"), client.getNickname()));
		return;
	}
	std::vector<std::string> args = splitByDelm(request[0], ",");
	std::string message = is_present(request, 1) ? request[1] : "";
	if (!message.empty() && message.find(":") == std::string::npos) {
		return;
	}
	for (size_t i = 0; i < args.size(); i++) {
		std::vector<Channel>::iterator it = findChannelByName(args[i]);
		std::cout << "Channel: " << args[i] << std::endl;
		if (it != channels.end()) {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&>::iterator u = findUser(users, client.getNickname());
			if (client.getNickname() == it->getCreator().getNickname()) {
				it->sendToAllUsers(client, formatUserMessage(client.getNickname(),
				client.getUserName(), host) + "PART " + args[i] + " " + message + "\r\n",false);
			
			}
			else if (u != users.end()) {
				it->sendToAllUsers(client, formatUserMessage(client.getNickname(),
				client.getUserName(), host) + "PART " + args[i] + " " + message + "\r\n",true);
				it->remove_user(u->second);
				client.decrementChannelCount();
				if (it->isAnOperatorOrOwner(u->second))
					it->remove_operator(u->second);
			} else {
				send_message(client.getFdNumber(), msg + ERR_NOTONCHANNEL(args[i]));
			}
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(args[i]));
		}
	}
}

void irc_server::NOTICE(std::vector<std::string> request, Client& client) {
	//  Numeric Replies:

    //        ERR_NORECIPIENT: [D]                 ERR_NOTEXTTOSEND: [D] --> privmsg #mad
    //        ERR_CANNOTSENDTOCHAN: [D]            ERR_NOTOPLEVEL: [X]
    //        ERR_WILDTOPLEVEL: [X]                ERR_TOOMANYTARGETS: [D] --> NOTICE ayoub,omar :hey -> split users with , not space
    //        ERR_NOSUCHNICK: [D]
    //        RPL_AWAY: [N] -> check if user is away
	bool too_may_args = false;
	std::string targets;
	
	for (size_t i = 0; i < request.size(); i++) {
		std::size_t pos = request[i].find(",");
		while (pos != std::string::npos) {
			too_may_args = true;
			targets += (request[i].substr(0, pos) + " ");
			request[i].erase(0, pos + 1);
			pos = request[i].find(",");
		}
		if (request[i].find(":") == std::string::npos) {
			targets += request[i].substr(0, request[i].find(" "));
		}
	}
	if (too_may_args) {
		send_message(client.getFdNumber(), msg + ERR_TOOMANYTARGETS(targets));
		return ;
	}

	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NORECIPIENT(std::string("NOTICE")));
		return ;
	} else if (!is_present(request, 1) || request[1].empty()) {
		send_message(client.getFdNumber(), msg + ERR_NOTEXTTOSEND());
		return ;
	}
	if (!checkChannelMask(request[0][0])) {
		std::vector<Channel>::iterator it = findChannelByName(request[0]);
		logger.log(DEBUG, "Send msg: " + request[1] + " To channel: |" + request[0] + "|");
		if (it != channels.end()) {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&>::iterator u = findUser(users, client.getNickname());
			if (client.getNickname() != it->getCreator().getNickname() && u == users.end()) {
				send_message(client.getFdNumber(), msg + ERR_CANNOTSENDTOCHAN(client.getNickname(), request[0]));
				return ;
			}
			std::cout << "users size: " << users.size() << std::endl;
			// ========================================================================================
			if (client.getFdNumber() != it->getCreator().getFdNumber()) {
				send_message(it->getCreator().getFdNumber(), formatUserMessage(client.getNickname(),
						client.getUserName(), host) + "PRIVMSG " + request[0] + " " + request[1] + "\r\n");
			}
			for (std::map<std::string, Client&>::iterator it = users.begin(); it != users.end(); it++) {
				if (client.getFdNumber() != it->second.getFdNumber()) {
					send_message(it->second.getFdNumber(), formatUserMessage(client.getNickname(),
						client.getUserName(), host) + "NOTICE " + request[0] + " " + request[1] + "\r\n");
				}
			}
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(request[0]));
		}
	} else {
		std::map<int, Client>::iterator it = findClient(request[0]);
		if (it != guest.end()) {
			send_message(it->first, formatUserMessage(client.getNickname(),
				client.getUserName(), host) + "NOTICE " + it->second.getNickname() + " " + request[1] + "\r\n");
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(client.getNickname(), request[0]));
		}
	}
}

void irc_server::LIST(std::vector<std::string> request, Client& client) {
	if (request.empty()) {
		std::string rpl = formatUserMessage(client.getNickname(), client.getUserName(), host);
		for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
			send_message(client.getFdNumber(), rpl + RPL_LIST(it->getChannelName(), it->getTopic()));
		}
		send_message(client.getFdNumber(), rpl + RPL_LISTEND(client.getNickname()));
	} else {
		std::vector<std::string> args = splitByDelm(request[0], ",");
		std::string rpl = formatUserMessage(client.getNickname(), client.getUserName(), host);
		for (size_t i = 0; i < args.size(); i++) {
			std::vector<Channel>::iterator it = findChannelByName(args[i]);
			if (it != channels.end()) {
				send_message(client.getFdNumber(), rpl + RPL_LIST(it->getChannelName(), it->getTopic()));
			}
		}
		send_message(client.getFdNumber(), rpl + RPL_LISTEND(client.getNickname()));
	}
}

void irc_server::QUIT(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("QUIT"), client.getNickname()));
		return ;
	}
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
		if (it->hasUser(client.getNickname())) {
			logger.log(DEBUG, "Channel name: " + it->getChannelName());
			it->sendToAllUsers(client, formatUserMessage(client.getNickname(),
				client.getUserName(), host) + "QUIT " + request[0] + "\r\n", true);
			if (it->isAnOperatorOrOwner(client)) {
				it->remove_operator(client);
			}
			it->remove_user(client);
		}
	}
	send_message(client.getFdNumber(), std::string("ERROR :Closing Link " + host + " (QUIT: " + request[0] + ")\r\n"));
	close(client.getFdNumber());
	nicknames.erase(client.getNickname());
	guest.erase(client.getFdNumber());
}

// names #madrid
// :192.168.1.174 353 ayoub = #madrid :omar @ayoub 
// :192.168.1.174 366 ayoub #madrid :End of /NAMES list
void irc_server::NAMES(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0)) {
		for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
			std::map<std::string, Client&> users = it->getUsers();
			std::string rpl = it->getCreator().getNickname() + " ";
			for (std::map<std::string, Client&>::iterator u = users.begin(); u != users.end(); u++) {
				rpl += u->first + " ";
			}
			send_message(client.getFdNumber(), msg + RPL_NAMREPLY(client.getNickname(), it->getChannelName(), rpl));
			send_message(client.getFdNumber(), msg + RPL_ENDOFNAMES(it->getChannelName()));
		}
	} else {
		std::vector<std::string> args = splitByDelm(request[0], ",");
		for (size_t i = 0; i < args.size(); i++) {
			std::vector<Channel>::iterator it = findChannelByName(args[i]);
			if (it != channels.end()) {
				std::map<std::string, Client&> users = it->getUsers();
				// check if user is not invisible
				std::string rpl = it->getCreator().getNickname() + " ";
				for (std::map<std::string, Client&>::iterator u = users.begin(); u != users.end(); u++) {
					rpl += u->first + " ";
				}
				send_message(client.getFdNumber(), msg + RPL_NAMREPLY(client.getNickname(), it->getChannelName(), rpl));
				send_message(client.getFdNumber(), msg + RPL_ENDOFNAMES(it->getChannelName()));
			}
		}
	}
}

void irc_server::OPER(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0) || !is_present(request, 1)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("OPER"), client.getNickname()));
		return ;
	}
	if (request[0].compare(0, request[0].length(), server_pass) || request[1] != client.getNickname()) {
		send_message(client.getFdNumber(), msg + ERR_PASSWDMISMATCH(client.getNickname(), client.getUserName(), host));
		return ;
	}
	client.setServerOper(true);
	send_message(client.getFdNumber(), RPL_YOUREOPER(client.getNickname()));
}

// sender -> :ayoub!ayoub@192.168.1.174 NOTICE reciever -> lab :p

void irc_server::WALLOPS(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("WALLOPS"), client.getNickname()));
	} else {
		if (!client.isServerOper()) {
			send_message(client.getFdNumber(), msg + ERR_NOPRIVILEGES(client.getNickname()));
			return ;
		}
		std::string param = request[0];
		if (param.find(":") == std::string::npos) {
			param = ":" + request[0];
		}
		std::string rpl = formatUserMessage(client.getNickname(), client.getUserName(), getHostAddress()) + "NOTICE ";
		for (std::map<int, Client>::iterator it = guest.begin(); it != guest.end(); it++) {
			// check wallops user
			if (it->second.getFdNumber() != client.getFdNumber()) {
				send_message(it->first, rpl + it->second.getNickname() + " " +param + "\r\n");
			}
		}
	}
}
// Replies:
// ERR_NOSUCHSERVER              ERR_NONICKNAMEGIVEN
//            RPL_WHOISUSER                 RPL_WHOISCHANNELS
//            RPL_WHOISCHANNELS             RPL_WHOISSERVER
//            RPL_AWAY                      RPL_WHOISOPERATOR
//            RPL_WHOISIDLE                 ERR_NOSUCHNICK
//            RPL_ENDOFWHOIS
// - Replies 311 - 313, 317 - 319 are all replies
//            generated in response to a WHOIS message.  Given that
//            there are enough parameters present, the answering
//            server MUST either formulate a reply out of the above
//            numerics (if the query nick is found) or return an
//            error reply.  The '*' in RPL_WHOISUSER is there as
//            the literal character and not as a wild card.  For
//            each reply set, only RPL_WHOISCHANNELS may appear
//            more than once (for long lists of channel names).
//            The '@' and '+' characters next to the channel name
//            indicate whether a client is a channel operator or
//            has been granted permission to speak on a moderated
//            channel.  The RPL_ENDOFWHOIS reply is used to mark
//            the end of processing a WHOIS message.

// :192.168.1.174 311 ayoub omar omar 192.168.1.174 * :Omar
// :192.168.1.174 319 ayoub omar :#madrid 
// :192.168.1.174 312 ayoub omar 192.168.1.174 :ft_irc
// :192.168.1.174 317 ayoub omar 4795 1686344801 :seconds idle
// :192.168.1.174 318 ayoub omar :End of /WHOIS list

// whois ayoub
// :192.168.1.174 311 omar ayoub 192.168.1.174 * :Ayoub
// :192.168.1.174 312 omar ayoub 192.168.1.174 :irc_server 
// :192.168.1.174 317 omar ayoub 123445678 :seconds idle
// :192.168.1.174 319 omar ayoub :#madrid @&test 
// :192.168.1.174 318 omar ayoub :End of WHOIS list
void irc_server::WHOIS(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NONICKNAMEGIVEN(client.getNickname()));
	} else {
		std::vector<std::string> args = splitByDelm(request[0], ",");
		std::cout << "------------ size ======= " << args.size() << "\n";
		for (size_t i = 0; i < args.size(); i++) {
			logger.log(DEBUG, "----> " + args[i]);
			std::map<int, Client>::iterator u = findClient(args[i]);
			if (u != guest.end()) {
				send_message(client.getFdNumber(), msg + RPL_WHOISUSER(client.getNickname(), u->second.getNickname(),
						getHostAddress(), u->second.getRealName()));
				send_message(client.getFdNumber(), msg + RPL_WHOISSERVER(client.getNickname(), u->second.getNickname(),
						getHostAddress(), "irc_server"));
				if (u->second.isServerOper()) {
					send_message(client.getFdNumber(), msg + RPL_WHOISOPERATOR(client.getNickname(), u->second.getNickname()));
				}
				std::stringstream ss;
				ss << std::difftime(time(0), u->second.getJoiningTime());
				send_message(client.getFdNumber(), msg + RPL_WHOISIDLE(client.getNickname(),
							u->second.getNickname(), ss.str() + std::string(" ") + u->second.getJoiningTimeAsString()));
				// channels
				std::vector<Channel> userChannels = findChannelsUserBelongTo(u->second);
				std::string arg;
				for (std::vector<Channel>::iterator it = userChannels.begin(); it != userChannels.end(); it++) {
					if (it->isAnOperatorOrOwner(u->second)) {
						arg += "@" + it->getChannelName() + " ";
					} else {
						arg += it->getChannelName() + " ";
					}
				}
				std::size_t pos = arg.find_last_of(" ");
				if (pos != std::string::npos)
					arg.erase(pos + 1);
				if (!userChannels.empty())
					send_message(client.getFdNumber(), msg + RPL_WHOISCHANNELS(client.getNickname(), u->second.getNickname(), arg));
			} else {
				send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(client.getNickname(), args[i]));
			}
			send_message(client.getFdNumber(), msg + RPL_ENDOFWHOIS(client.getNickname(), u->second.getNickname()));
		}
	}
}

int	irc_server::getSocketFd(void){
	return (this->socket_fd);
}

void	irc_server::setSocketFd(int socket_fd){
	this->socket_fd = socket_fd;
}

void	irc_server::setPassword(std::string passwd){
	this->passwd = passwd;
}

std::string	irc_server::getPassword(){
	return(this->passwd);
}

std::vector<Channel> irc_server::findChannelsUserBelongTo(Client& client) {
	std::vector<Channel> vec;
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
		std::map<std::string, Client&> users = it->getUsers();
		std::map<std::string, Client&>::iterator u = findUser(users, client.getNickname());
		if (u != users.end() || client.getNickname() == it->getCreator().getNickname()) {
			vec.push_back(*it);
		}
	}
	return vec;
}

std::vector<Channel>::iterator irc_server::findChannelByName(std::string channel) {
	for (std::vector<Channel>::iterator it = channels.begin() ; it != channels.end(); it++) {
		std::cout << "My channel_name: " << it->getChannelName() << "|" << it->getChannelName().length() << "| Incoming channel:" << channel << "|" << channel.length() << "|" << std::endl;
		if (!it->getChannelName().compare(0, it->getChannelName().length(), channel)) {
			std::cout << "My channel_name: " << it->getChannelName() << " Incoming channel: " << channel << std::endl;
			return it;
		}
	}
	return channels.end();
}

std::map<int, Client>::iterator irc_server::findClient(std::string nickname) {
	std::map<int, Client>::iterator it = guest.begin();
	for (; it != guest.end(); it++) {
		if (it->second.getNickname() == nickname) {
			return it;
		}
	}
	return guest.end();
}

std::map<std::string, Client&>::iterator irc_server::findUser(std::map<std::string, Client&>& users, std::string nickname) {
	std::map<std::string, Client&>::iterator u = users.end();
	for (std::map<std::string, Client&>::iterator ut = users.begin(); ut != users.end(); ut++) {
		if (ut->first == nickname) {
			u = ut;
			break ;
		}
	}
	return u;
}

int irc_server::is_present(std::vector<std::string> args, int index) {
	try {
		if (args.at(index).empty()) {
			return 0;
		}
		return 1;
	} catch(std::out_of_range e) {
		return 0;
	}
}

std::string irc_server::formatUserMessage(std::string nickname, std::string username, std::string hostname) {
	return ":" + nickname + "!" + username + "@" + hostname + " ";
}

bool irc_server::checkMode(char c) {
	return (c != 'k' && c != 'i' && c != 'l' && c != 'o');
}

std::vector<std::pair<int, Arg> > irc_server::checkAction(std::string mode) {
	// t, k, o, i
	// k-i+ol -> i = 0, j = 0,
	// j = 1, i = 1
	// i = 2, j = 1
	// i = 3, j = 2
	// i = 4, j = 2
	// i = 5, j = 2
	std::vector<std::pair<int, Arg> > vec;
	// char op = '+';
	char prev = mode[0] == '+' ? '+' : mode[0] != '-' ? '+' : '-';
	// std::string m;
	size_t i = 0;
	size_t j = 0;
	// std::cout << "mode: " << mode << std::endl;
	while (i < mode.length()) {
		// prev = mode[i] == '+' ? '+' : mode[i] != '-' ? '+' : '-';
		// if (i == 0 && mode[i] != '-' && mode[i] != '+') {
		// 	vec.push_back(std::pair<int, Arg>(checkMode(mode[i]) ? E_UNKNOWN : E_ADD, Arg(mode[i], 0)));
		// 	i++;
		// }
		if (checkMode(mode[i])) {
			j += 1;
			prev = mode[i];
			if (prev != '+' && prev != '-') {
				Arg arg = Arg(prev, i - j);
				vec.push_back(std::pair<int, Arg>(E_UNKNOWN, arg));
			}
			// std::cout << "prev: " << prev << std::endl;
			i++;
		} else {
			// +k-i
			Arg arg = Arg(mode[i], i - j);
			// if (i != 0)
			// 	prev = (mode[i - 1] != '-' && mode[i - 1] != '+') && (mode[i] != '+' && mode[i] != '-') ? '+' : prev;
			vec.push_back(std::pair<int, Arg>(prev == '+' ? E_ADD : prev == '-' ? E_REMOVE : E_UNKNOWN, arg));
			i++;
		}
	}
	return vec;
	// return op == '+' ? std::pair<e_action, std::string>(ADD, "") : op == '-' ? std::pair<e_action, std::string>(REMOVE, "") : std::pair<e_action, std::string>(UNKOWN, "");
}

void irc_server::leaveAllChannels(Client& client) {
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
		if (it->hasUser(client.getNickname())) {
			if (it->isAnOperatorOrOwner(client))
				it->remove_operator(client);
			it->remove_user(client);
		}
	}
}

std::string irc_server::getHostAddress(){
    std::system( "ifconfig | grep 'inet ' | sed -n 2p | awk '{print $2}' > /tmp/irc_server.log" );
	std::stringstream ss;
	ss << std::ifstream( "/tmp/irc_server.log" ).rdbuf();
	return (ss.str().substr( 0, ss.str().find( '\n' ) ));
}

irc_server::irc_server(): channels(), spotnuts(Bot::getBot()), logger(Logger::getLogger()), filetransfer(FileTransfer::getInstance()) {
	server_pass = "Admin@1337";
	host = getHostAddress();
	msg = ":" + host + " ";
}

irc_server::~irc_server(){
}

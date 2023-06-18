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
						logger.log(DEBUG, "Message " + _message);
						object.parseRequest(_message);
						if (!object.getcmd().compare(0, object.getcmd().length(), "PASS"))
							PASS(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "NICK"))
							NICK(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "USER"))
							USER(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "PONG"))
							PONG(object.getRequest(), guest[vec_fd[i].fd]);
						else {
							if (!guest[vec_fd[i].fd].getUserName().empty()) {
								if (!object.getcmd().compare(0, object.getcmd().length(), "KICK"))
									KICK(object.getRequest(), guest[vec_fd[i].fd]);
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
								else if (!object.getcmd().compare(0, object.getcmd().length(), "QUIT")) {
									QUIT(object.getRequest(), guest[vec_fd[i].fd]);
									leaveAllChannels(guest[vec_fd[i].fd]);
									nicknames.erase(guest[vec_fd[i].fd].getNickname());
									guest[vec_fd[i].fd].setPasswordApproved(false);
									guest[vec_fd[i].fd].setNicknameSited(false);
									guest[vec_fd[i].fd].setUsernameSited(false);
									close(vec_fd[i].fd);
									vec_fd.erase(vec_fd.begin() + i);
								}
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
									send_message(vec_fd[i].fd, msg + ERR_UNKNOWNCOMMAND(Client_data.getNickname(), object.getcmd()));
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


void irc_server::PASS(std::vector<std::string> request, Client &client)
{
	if (!request.size()){
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("PASS"), client.getNickname()));
		return;
	}
	else if (request[0] != this->passwd){
		send_message(client.getFdNumber(), msg + ERR_PASSWDMISMATCH(client.getNickname()));
		return;
	}
	else if (request[0] == this->passwd && !client.getPasswordApproved()){
		std::cout << "Password approved, you can set a nickname" << std::endl;
		client.setPasswordApproved(true);
		return ;
	}
	else if (client.getPasswordApproved() == true){
		send_message(client.getFdNumber(), msg + ERR_ALREADYREGISTRED(client.getNickname()));
		return ;
	}
}

void irc_server::NICK(std::vector<std::string> request, Client &client)
{
	if (!request.size())
	{
		send_message(client.getFdNumber(), msg + ERR_NONICKNAMEGIVEN(client.getNickname()));
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
			send_message(client.getFdNumber(), msg + ERR_NICKNAMEINUSE(request[0]));
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
				client.set_wallops(true);
				welcome_message(client.getFdNumber(), msg + RPL_WELCOME(client.getNickname(), client.getUserName(), host));
				welcome_message(client.getFdNumber(), msg + RPL_YOURHOST(client.getNickname(), host));
				get_date(client);
			}
		}
		else
			send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname()));
	}
}

bool irc_server::checkChannelMask(char c) {
	return (c != '#');
}


void irc_server::JOIN(std::vector<std::pair<std::string, std::string> > _request, Client& client) {
	if (_request.empty()) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("JOIN"), client.getNickname()));
		return ;
	}
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
				std::vector<Channel>::iterator it = findChannelByName(pair.first);
				it->buildJoinChannelRes(msg, client);
			}
		}
	}
}

void irc_server::PONG(std::vector<std::string> request, Client& client) {
	(void)client;
	(void)request;
}

void irc_server::PRIVMSG(std::vector<std::string> request, Client& client) {
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
			pos = request[i].find(" ");
			if (pos != std::string::npos)
				targets += request[i].substr(0, pos);
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
	std::string message = request[1];
	if (message.find(":") == std::string::npos) {
		message = ":" + request[1];
	}
	if (!checkChannelMask(request[0][0])) {
		std::vector<Channel>::iterator it = findChannelByName(request[0]);
		logger.log(DEBUG, "Send msg: " + message + " To channel: |" + request[0] + "|");
		if (it != channels.end()) {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&> operators = it->getOperators();
			std::map<std::string, Client&>::iterator u = findUser(users, client.getNickname());
			std::map<std::string, Client&>::iterator oper = findOperator(operators, it->getCreator().getNickname());
			if (it->hasOwner() && client.getNickname() != it->getCreator().getNickname() && u == users.end()) {
				send_message(client.getFdNumber(), msg + ERR_CANNOTSENDTOCHAN(client.getNickname(), request[0]));
				return ;
			}
			if (oper != operators.end() && oper->second.getNickname() != client.getNickname()) {
				send_message(oper->second.getFdNumber(), formatUserMessage(client.getNickname(), client.getUserName(), host) + "PRIVMSG " + request[0] + " " + message + "\r\n");
			}
			it->sendToAllUsers(client, formatUserMessage(client.getNickname(), client.getUserName(), host) + "PRIVMSG " + request[0] + " " + message + "\r\n", false);
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(request[0]));
		}
	} else {
		std::map<int, Client>::iterator it = findClient(request[0]);
		if (it != guest.end()) {
			send_message(it->first, formatUserMessage(client.getNickname(),
				client.getUserName(), host) + "PRIVMSG " + it->second.getNickname() + " " + message + "\r\n");
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(client.getNickname(), request[0]));
		}
	}
}


void	irc_server::KICK(std::vector<std::string> _request, Client& client) {
	logger.log(INFO, "kick command for channel");
	std::cout << "Size: " << _request[0] << std::endl;
	if (!is_present(_request, 0) || !is_present(_request, 1)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("KICK"), client.getNickname()));
		return ;
	}
	if (checkChannelMask(_request[0][0]) || _request[0].find(",") != std::string::npos
		|| _request[0].find(" ") != std::string::npos) {
		send_message(client.getFdNumber(), msg + ERR_BADCHANMASK(client.getNickname(), _request[0]));
	} else {
		std::vector<Channel>::iterator it = findChannelByName(_request[0]);
		if (it != channels.end()) {
			if (client.getNickname() == _request[1]) {
				return ;
			}
			if (it->hasUser(client.getNickname())) {
				if (it->isAnOperatorOrOwner(client)) {
					std::map<std::string, Client&> users = it->getUsers();
					std::map<std::string, Client&> operators = it->getOperators();
					std::map<std::string, Client&>::iterator u = findUser(users, _request[1]);
					std::map<std::string, Client&>::iterator oper = findUser(operators, _request[1]);
					if (u != users.end()) {
						it->sendToAllUsers(client, formatUserMessage(client.getNickname(), client.getUserName(), host) + "KICK " + it->getChannelName() + " " + _request[1] + " :" + client.getNickname() + "\r\n", true);
						it->remove_user(u->second);
					}
					if (oper != operators.end()) {
						it->sendToAllUsers(client, formatUserMessage(client.getNickname(), client.getUserName(), host) + "KICK " + it->getChannelName() + " " + _request[1] + " :" + client.getNickname() + "\r\n", true);
						it->remove_operator(oper->second);
					}
					if (u == users.end() && oper == operators.end()) {
						send_message(client.getFdNumber(), msg + ERR_USERNOTINCHANNEL(client.getNickname(), _request[1]));
					}
				} else {
					send_message(client.getFdNumber(), msg + ERR_CHANOPRIVSNEEDED(_request[1]));
				}
			} else {
				send_message(client.getFdNumber(), msg + ERR_NOTONCHANNEL(_request[1]));
			}
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(_request[1]));
		}
	}
}


void	irc_server::INVITE(std::vector<std::string> request, Client& client) 
{	
	logger.log(DEBUG, "INVITE");
	std::cout << "request: " << request[0] << std::endl;
	if(request.size() == 2)
	{	
		std::vector<Channel>::iterator it = findChannelByName(request[1]);
		if(it != channels.end())
		{
			logger.log(DEBUG, "Found a channel!");
			std::map<int, Client>::iterator cl = findClient(client.getNickname());
			if(it->hasUser(cl->second.getNickname()))
			{
				std::map<int, Client>::iterator it2 = findClient(request[0]);
				if (it2 != guest.end())
				{ 
					if(it->isInviteOnly())
					{
						logger.log(DEBUG, "This channel is Invite Only");
						if(it->isAnOperatorOrOwner(client))
						{
							logger.log(DEBUG, "You're an Operator! You can Proceed ...");
							if (it2->second.getNickname() != client.getNickname())
							{
								it->invite_user(it2->second);
								send_message(client.getFdNumber(), msg + RPL_INVITING(request[1], request[0]));
								send_message(it2->first, msg + std::string(client.getNickname() + " NOTICE :has sent you an invite\r\n"));
							}
						}
						else 
							send_message(client.getFdNumber(), msg + ERR_CHANOPRIVSNEEDED(request[1]));
						
					}
				}
				else
					send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(request[1], request[0]));
			}
			else
				send_message(client.getFdNumber(), msg + ERR_NOTONCHANNEL(it->getChannelName()));
		}
		else
			send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(request[1], request[0]));
	}
	else
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("INVITE"), client.getNickname()));
	return ;
}

void	irc_server::TOPIC(std::vector<std::string> request, Client& client)
{
	logger.log(DEBUG, "TOPIC");
	
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("TOPIC"), client.getNickname()));
		return ;
	}
	std::vector<Channel>::iterator it = findChannelByName(request[0]);
	if(it != channels.end())
	{	
		if(it->hasUser(client.getNickname()))
		{
			if (!it->getSupportTopic()) {
				send_message(client.getFdNumber(), msg + ERR_NOCHANMODES(client.getNickname(), it->getChannelName()));
				return;
			}
			if(it->isAnOperatorOrOwner(client))
			{
				if(!is_present(request, 1) || request[1].empty())
				{
					if(it->getChannelTopic().empty())
						logger.log(DEBUG, "Channel " + request[0] + " has no topic yet");
					else
					{
						logger.log(DEBUG, "user " + client.getNickname() + " checking topic for Channel " + request[0]);
						std::cout << "- " << it->getChannelTopic() << std::endl;
					}
					send_message(client.getFdNumber(), msg + RPL_TOPIC(client.getNickname(), it->getChannelName(), it->getChannelTopic()));
				}
				else
				{
					if(request[1][0] == ':')
					{
						logger.log(DEBUG, "Found token!");
						if(request[1][1])
						{
							it->setChannelTopic(&request[1][1]);
							it->sendToAllUsers(client, msg + RPL_TOPIC(client.getNickname(), it->getChannelName(), it->getChannelTopic()), true);
							logger.log(DEBUG, "user " + client.getNickname() + " setting topic for Channel " + request[0] + " to " + it->getChannelTopic());
						}
						else
						{
							it->setChannelTopic("");
							it->sendToAllUsers(client, msg + RPL_NOTOPIC(client.getNickname(), it->getChannelName()), true);
							logger.log(DEBUG, "user " + client.getNickname() + " cleared topic for " + request[0]);
						}
					}
				}
			}
			else {
				if (request[1][0] == ':') {
					send_message(client.getFdNumber(), msg + ERR_CHANOPRIVSNEEDED(it->getChannelName()));
					return;
				}
				send_message(client.getFdNumber(), msg + RPL_TOPIC(client.getNickname(), it->getChannelName(), it->getChannelTopic()));
			}
		}
		else
			send_message(client.getFdNumber(), msg + ERR_NOTONCHANNEL(it->getChannelName()));
	}
	else
		logger.log(DEBUG, "Channel Not found");
	return ;
}


void	irc_server::SENDFILE(std::vector<std::string> request, Client& client) {
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
	std::string reply = msg + client.getNickname() + " : Files\r\n";
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
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("MODE"), client.getNickname()));
		return;
	}
	if (request[0][0] == '#') {
		logger.log(DEBUG, "Basic implementation of mode operations on channels");
		std::vector<Channel>::iterator it = findChannelByName(request[0]);
		if (it != channels.end()) {
			logger.log(DEBUG, "Channel found " + request[0]);
			if (it->isAnOperatorOrOwner(client)) {
				logger.log(DEBUG, "user is an actual owner or operator of the channel");
				if (!is_present(request, 1)) {
					send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("MODE"), client.getNickname()));
					return;
				}
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
		logger.log(DEBUG, "Basic implementation of mode operations on users");
		std::map<int, Client>::iterator cl = findClient(request[0]);
		if (cl != guest.end()) 
		{
			if(request[0] != client.getNickname()) {
				send_message(client.getFdNumber(), msg + ERR_USERSDONTMATCH);
			}
			else if (is_present(request, 1)) {
				std::vector<std::pair<int, Arg> > actions = checkUserAction(request[1]);
				for (size_t i = 0; i < actions.size(); i++) {
					std::pair<int, Arg> pair = actions[i];
					std::cout << "mode: " << pair.first << " action: " << pair.second.getChar() << "\n";
					if (pair.first == E_UNKNOWN) {
						send_message(client.getFdNumber(), msg + ERR_UMODEUNKNOWNFLAG(client.getNickname()));
					} else if (pair.first == E_ADD) {
						add_user_mode(client, pair.second.getChar());
					} else {
						remove_user_mode(client, pair.second.getChar());
					}
				}
			} else {
				send_message(client.getFdNumber(), msg + RPL_UMODEIS(client.getNickname(), buildMode(client)));
			}
		}
	}
}

std::string irc_server::buildMode(Client& client) {
	std::string mode;
	if (client.get_invisible())
		mode += "+i,";
	else
		mode += "-i,";
	if (client.get_wallops())
		mode += "+w,";
	else
		mode += "-w,";
	if (client.isServerOper())
		mode += "+o,";
	else
		mode += "-o,";
	if (client.get_rest_conx())
		mode += "+r,";
	else
		mode += "-r,";
	mode = mode.substr(0, mode.find_last_of(","));
	return mode;
}

void irc_server::add_user_mode(Client& client, char mode) {
	std::string message = ":" + client.getNickname() + " MODE " + client.getNickname() + " :+" + mode + "\r\n";
	if (mode != 'o') {
		if (mode == 'i') {
			logger.log(DEBUG, "Adding invisible mode ...");
			client.set_invisible(true);
			client.get_user_mode().push_back(u_i);
		}
		else if (mode == 'w')
		{
			logger.log(DEBUG, "Adding recieve wallops mode ...");
			client.set_wallops(true);
			client.get_user_mode().push_back(u_w);
		}
		else if (mode == 'r')
		{
			logger.log(DEBUG, "Adding restricted connection mode ...");
			client.set_rest_conx(true);
			client.get_user_mode().push_back(u_r);
		}
		send_message(client.getFdNumber(), message);
	}
}

void irc_server::remove_user_mode(Client& client, char mode) {
	std::string message = ":" + client.getNickname() + " MODE " + client.getNickname() + " :-" + mode + "\r\n";
	if (mode == 'i') {
		logger.log(DEBUG, "Removing invisible mode ...");
		client.set_invisible(false);
	}
	else if (mode == 'w')
	{
		logger.log(DEBUG, "Removing recieve wallops mode ...");
		client.set_wallops(false);
	}
	else if (mode == 'r')
	{
		logger.log(DEBUG, "Removing restricted connection mode ...");
		client.set_rest_conx(false);
	}
	else if (mode == 'o') {
		logger.log(DEBUG, "Removing server operator mode ...");
		client.setServerOper(false);
	}
	send_message(client.getFdNumber(), message);
}

void irc_server::add_mode(Client& client, std::vector<Channel>::iterator& it, std::pair<int, Arg> pair, std::vector<std::string> request) {
	logger.log(DEBUG, "mode prefix ADD");
	int index = pair.second.getIndex();
	std::string message = formatUserMessage(client.getNickname(), client.getUserName(), host) + "MODE " + it->getChannelName() + " +" + pair.second.getChar();
	if (pair.second.getChar() == 'i') {
		it->setInviteOnly(true);
		it->sendToAllUsers(client, message + "\r\n", true);
	}
	if (pair.second.getChar() == 't') {
		it->setSupportTopic(true);
		it->sendToAllUsers(client, message + "\r\n", true);
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
				it->sendToAllUsers(client, message + " " + request[index] + "\r\n", true);
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
						it->sendToAllUsers(client, message + " " + request[index] + "\r\n", true);
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
			it->sendToAllUsers(client, message + " " + request[index] + "\r\n", true);
		}
	}
}

void irc_server::remove_mode(Client& client, std::vector<Channel>::iterator& it, std::pair<int, Arg> pair, std::vector<std::string> request) {
	logger.log(DEBUG, "mode prefix REMOVE");
	std::string message = formatUserMessage(client.getNickname(), client.getUserName(), host) + "MODE " + it->getChannelName() + " -" + pair.second.getChar();
	int index = pair.second.getIndex();
	if (pair.second.getChar() == 'i') {
		it->setInviteOnly(false);
		it->sendToAllUsers(client, message + "\r\n", true);
	}
	if (pair.second.getChar() == 't') {
		it->setSupportTopic(false);
		it->sendToAllUsers(client, message + "\r\n", true);
	}
	if (pair.second.getChar() == 'k') {
		if (!is_present(request, index)) {
			logger.log(DEBUG, "Error here");
			send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("MODE"), client.getNickname()));
		} else if (it->getChannelKey().compare(0, it->getChannelKey().length(), request[index])) {
			logger.log(ERROR, "unmatched keys");
		} else {
			it->setChannelKey("");
			it->sendToAllUsers(client, message + " " + request[index] + "\r\n", true);
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
						it->sendToAllUsers(client, message + " " + request[index] + "\r\n", true);
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
		it->sendToAllUsers(client, message + "\r\n", true);
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
	bool found = false;
	for (size_t i = 0; i < args.size(); i++) {
		std::vector<Channel>::iterator it = findChannelByName(args[i]);
		std::cout << "Channel: " << args[i] << std::endl;
		if (it != channels.end()) {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&> operators = it->getOperators();
			std::map<std::string, Client&>::iterator u = findUser(users, client.getNickname());
			for (std::map<std::string, Client&>::iterator u = operators.begin(); u != operators.end(); u++) {
				if (it->getCreator().getNickname() == client.getNickname()) {
					it->sendToAllUsers(client, formatUserMessage(client.getNickname(),
						client.getUserName(), host) + "PART " + args[i] + " " + message + "\r\n", true);
					it->remove_operator(client);
					found = true;
					break;
				}
			}
			if (u != users.end()) {
				it->sendToAllUsers(client, formatUserMessage(client.getNickname(),
				client.getUserName(), host) + "PART " + args[i] + " " + message + "\r\n",true);
				it->remove_user(u->second);
				client.decrementChannelCount();
				if (it->isAnOperatorOrOwner(u->second))
					it->remove_operator(u->second);
			} else if (!found) {
				send_message(client.getFdNumber(), msg + ERR_NOTONCHANNEL(args[i]));
			}
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(args[i]));
		}
	}
}

void irc_server::NOTICE(std::vector<std::string> request, Client& client) {
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
	std::string message = request[1];
	if (message.find_first_of(":") == std::string::npos) {
		message = ":" + request[1];
	}
	if (!checkChannelMask(request[0][0])) {
		std::vector<Channel>::iterator it = findChannelByName(request[0]);
		logger.log(DEBUG, "Send msg: " + request[1] + " To channel: |" + request[0] + "|");
		if (it != channels.end()) {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&> operators = it->getOperators();
			std::map<std::string, Client&>::iterator u = findUser(users, client.getNickname());
			std::map<std::string, Client&>::iterator oper = findOperator(operators, it->getCreator().getNickname());
			if (client.getNickname() != it->getCreator().getNickname() && u == users.end()) {
				send_message(client.getFdNumber(), msg + ERR_CANNOTSENDTOCHAN(client.getNickname(), request[0]));
				return ;
			}
			if (oper != operators.end() && oper->second.getNickname() != client.getNickname()) {
				send_message(oper->second.getFdNumber(), formatUserMessage(client.getNickname(), client.getUserName(), host) + "NOTICE " + request[0] + " " + message + "\r\n");
			}
			it->sendToAllUsers(client, formatUserMessage(client.getNickname(), client.getUserName(), host) + "NOTICE " + request[0] + " " + message + "\r\n", false);
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHCHANNEL(request[0]));
		}
	} else {
		std::map<int, Client>::iterator it = findClient(request[0]);
		if (it != guest.end()) {
			send_message(it->first, formatUserMessage(client.getNickname(),
				client.getUserName(), host) + "NOTICE " + it->second.getNickname() + " " + message + "\r\n");
		} else {
			send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(client.getNickname(), request[0]));
		}
	}
}

void irc_server::LIST(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0) || request[0].empty()) {
		std::string rpl = msg + RPL_LISTSTART(client.getNickname());
		for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
			std::stringstream s;
			s << it->getUsers().size() + 1;
			rpl += msg + RPL_LIST(it->getChannelName(), client.getNickname(), s.str(), it->getChannelTopic());
		}
		rpl += msg + RPL_LISTEND(client.getNickname());
		send_message(client.getFdNumber(), rpl);
	} else {
		std::vector<std::string> args = splitByDelm(request[0], ",");
		std::string rpl = msg + RPL_LISTSTART(client.getNickname());
		for (size_t i = 0; i < args.size(); i++) {
			std::vector<Channel>::iterator it = findChannelByName(args[i]);
			if (it != channels.end()) {
				std::stringstream ss;
				ss << it->getUsers().size() + 1;
				rpl += msg + RPL_LIST(it->getChannelName(), client.getNickname(), ss.str(), it->getChannelTopic());
			}
		}
		rpl += msg + RPL_LISTEND(client.getNickname());
		send_message(client.getFdNumber(), rpl);
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
}

void irc_server::NAMES(std::vector<std::string> request, Client& client) {
	if (!is_present(request, 0) || request[0].empty()) {
		for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
			std::map<std::string, Client&> users = it->getUsers();
			std::map<std::string, Client&> operators = it->getOperators();
			std::string rpl;
			for (std::map<std::string, Client&>::iterator u = users.begin(); u != users.end(); u++) {
				rpl += u->first + " ";
			}
			for (std::map<std::string, Client&>::iterator u = operators.begin(); u != operators.end(); u++) {
				if (it->getCreator().getNickname() == u->second.getNickname()) {
					rpl += "@" + u->second.getNickname();
					break;
				}
			}
			send_message(client.getFdNumber(), msg + RPL_NAMREPLY(client.getNickname(), it->getChannelName(), rpl));
			send_message(client.getFdNumber(), msg + RPL_ENDOFNAMES(client.getNickname(), it->getChannelName()));
		}
	} else {
		std::vector<std::string> args = splitByDelm(request[0], ",");
		for (size_t i = 0; i < args.size(); i++) {
			std::vector<Channel>::iterator it = findChannelByName(args[i]);
			if (it != channels.end()) {
				std::map<std::string, Client&> users = it->getUsers();
				std::map<std::string, Client&> operators = it->getOperators();
				std::string us;
				std::string rpl = msg + "353 " + client.getNickname() + " = " + it->getChannelName() + " :";
				for (std::map<std::string, Client&>::iterator u = users.begin(); u != users.end(); u++) {
					us += u->first + " ";
				}
				for (std::map<std::string, Client&>::iterator u = operators.begin(); u != operators.end(); u++) {
					if (it->getCreator().getNickname() == u->second.getNickname()) {
						us += "@" + u->second.getNickname();
						break;
					}
				}
				rpl += us + "\r\n";
				rpl += msg + RPL_ENDOFNAMES(client.getNickname(), it->getChannelName());
				send_message(client.getFdNumber(), rpl);
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
		send_message(client.getFdNumber(), msg + ERR_PASSWDMISMATCH(client.getNickname()));
		return ;
	}
	client.setServerOper(true);
	send_message(client.getFdNumber(), RPL_YOUREOPER(client.getNickname()));
}


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
			if (it->second.getFdNumber() != client.getFdNumber()) {
				if (it->second.get_wallops())
					send_message(it->first, rpl + it->second.getNickname() + " " +param + "\r\n");
			}
		}
	}
}


void irc_server::WHOIS(std::vector<std::string> request, Client& client) {
	std::cout << "request: " << request[0] << "\n";
	if (!is_present(request, 0)) {
		send_message(client.getFdNumber(), msg + ERR_NONICKNAMEGIVEN(client.getNickname()));
	} else {
		size_t pos = request[0].find(" ");
		std::vector<std::string> args = splitByDelm(pos != std::string::npos ? request[0].substr(pos + 1) : request[0], ",");
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
				send_message(client.getFdNumber(), msg + RPL_ENDOFWHOIS(client.getNickname()));
			} else {
				send_message(client.getFdNumber(), msg + ERR_NOSUCHNICK(client.getNickname(), args[i]));
			}
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

std::map<std::string, Client&>::iterator irc_server::findOperator(std::map<std::string, Client&>& operators, std::string nickname) {
	std::map<std::string, Client&>::iterator u = operators.end();
	for (std::map<std::string, Client&>::iterator ut = operators.begin(); ut != operators.end(); ut++) {
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
	return (c != 'k' && c != 'i' && c != 'l' && c != 'o' && c != 't');
}

bool irc_server::checkUserMode(char c) {
	return (c != 'w' && c != 'r' && c != 'o' && c != 'i');
}

std::vector<std::pair<int, Arg> > irc_server::checkUserAction(std::string mode) {
	std::vector<std::pair<int, Arg> > vec;
	char prev = mode[0] == '+' ? '+' : mode[0] != '-' ? '+' : '-';
	size_t i = 0;
	size_t j = 0;
	while (i < mode.length()) {
		if (checkUserMode(mode[i])) {
			j += 1;
			prev = mode[i];
			if (prev != '+' && prev != '-') {
				Arg arg = Arg(prev, i - j);
				vec.push_back(std::pair<int, Arg>(E_UNKNOWN, arg));
			}
			i++;
		} else {
			Arg arg = Arg(mode[i], i - j);
			vec.push_back(std::pair<int, Arg>(prev == '+' ? E_ADD : prev == '-' ? E_REMOVE : E_UNKNOWN, arg));
			i++;
		}
	}
	return vec;
}

std::vector<std::pair<int, Arg> > irc_server::checkAction(std::string mode) {
	std::vector<std::pair<int, Arg> > vec;
	char prev = mode[0] == '+' ? '+' : mode[0] != '-' ? '+' : '-';
	size_t i = 0;
	size_t j = 0;
	while (i < mode.length()) {
		if (checkMode(mode[i])) {
			j += 1;
			prev = mode[i];
			if (prev != '+' && prev != '-') {
				Arg arg = Arg(prev, i - j);
				vec.push_back(std::pair<int, Arg>(E_UNKNOWN, arg));
			}
			i++;
		} else {
			Arg arg = Arg(mode[i], i - j);
			vec.push_back(std::pair<int, Arg>(prev == '+' ? E_ADD : prev == '-' ? E_REMOVE : E_UNKNOWN, arg));
			i++;
		}
	}
	return vec;
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

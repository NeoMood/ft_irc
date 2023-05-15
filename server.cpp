/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:26:47 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/15 23:03:40 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "client.hpp"

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

void	irc_server::AcceptToIncomingconnection(Client& Client_data)
{
	char buffer[BUFFER_SIZE];
	for (size_t i = 0; i < vec_fd.size(); i++)
	{
		if (vec_fd[i].revents & POLLIN && i == 0)
		{
			struct sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
			if (accept_fd == -1){
				std::cerr << "accept: " << std::strerror(errno) << std::endl;
				exit (EXIT_FAILURE);
			}
			Client_data.setfd_number(accept_fd);
			Client_data.setip_adress(inet_ntoa(client_addr.sin_addr));
			guest.insert(std::make_pair(accept_fd, Client_data));
			pollfd client_fd;
			client_fd.fd = accept_fd;
			client_fd.events = POLLIN;
			vec_fd.push_back(client_fd);
		}
		else if (vec_fd[i].revents & POLLIN )
		{
			std::memset(&buffer, 0, sizeof(buffer));
			int nbytes = recv(vec_fd[i].fd, buffer, BUFFER_SIZE, 0);
			if (nbytes == -1) {
				std::cerr << "recv: " << std::strerror(errno) << std::endl;
				exit(EXIT_FAILURE);
			}
			else if (nbytes == 0) {
				close(vec_fd[i].fd);
				vec_fd.erase(vec_fd.begin() + i);
				std::cout << "See you later" << std::endl;
			}
			else 
			{
				std::string message(buffer);
				std::cout << message << std::endl;
				if (message.size() > message.max_size()){
					std::cerr << "Message Buffer low" << std::endl;
				}
				size_t	pos = message.find(" ");
				if (pos != std::string::npos){
					std::string	command = message.substr(0, pos);
					full_command.push_back(command);
					std::string	parameters = message.substr(pos + 1, message.length() - pos - 2);
					full_command.push_back(parameters);
					if (!command.compare(0, command.length(), "PASS")){
						PASS(full_command, Client_data);
					}
						
				}
				else{
					std::cerr << "Invalid" << std::endl;
				}
			}
		}	
	}
}


void	irc_server::multipleconnection(){
	Client	Client_data;
	pollfd server_fd;

	server_fd.fd = socket_fd;
	server_fd.events = POLLIN;
	vec_fd.push_back(server_fd);
	std::cout << "IRC Server is starting..." << std::endl;
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

irc_server::irc_server(){
}

irc_server::~irc_server(){
}

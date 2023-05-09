/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:26:47 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/09 18:49:26 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void irc_server::check_port(char **argv)
{
    // int port = std::stoi(argv[1]);
	// int	pass = std::stoi(argv[2]);
	std::string port = argv[1];
	std::string password = argv[2];
    if (port < "1024" || port > "65535")
        throw std::out_of_range("Port number out of the bounds");
	for (int i = 0; password[i]; i++){
		if (password[i] < '0' || password[i] > '9')
			throw std::invalid_argument("Password must be a digit");
	}
    this->portno = std::stoi(port);
    this->passwd = std::stoi(password);
}

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

void	irc_server::AcceptToIncomingconnection(void)
{
	for (size_t i = 0; i < vec_fd.size(); i++)
	{
		if (vec_fd[i].revents & POLLIN)
		{
			if (vec_fd[i].fd == socket_fd)
			{
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
				if (accept_fd == -1){
					std::cerr << "accept: " << std::strerror(errno) << std::endl;
					exit (EXIT_FAILURE);
				}
				char client_ip[INET_ADDRSTRLEN];
				std::cout << "New connection: " << inet_ntop(AF_INET,&(client_addr.sin_addr),client_ip, INET_ADDRSTRLEN) << std::endl;
				pollfd client_fd;

				client_fd.fd = accept_fd;
				client_fd.events = POLLIN;
				vec_fd.push_back(client_fd);
			}
			else
			{
				char buffer[BUFFER_SIZE] = {0};
				int nbytes = recv(vec_fd[i].fd, buffer, BUFFER_SIZE, 0);
				if (nbytes == -1) {
					std::cerr << "recv: " << std::strerror(errno) << std::endl;
					exit(EXIT_FAILURE);
				}
				else if (nbytes == 0) {
					std::cout << "See you later" << std::endl;
					exit (EXIT_SUCCESS);
				}
				else {
					std::cout << "Received: " << buffer << std::endl;
				}
			}
		}
	}
}

void	irc_server::multipleconnection(){
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
		AcceptToIncomingconnection();
	}
}

int	irc_server::getSocketFd(void){
	return (this->socket_fd);
}

void	irc_server::setSocketFd(int socket_fd){
	this->socket_fd = socket_fd;
}


irc_server::irc_server(){
	
}

irc_server::~irc_server(){
	
}

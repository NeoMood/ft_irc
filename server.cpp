/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:26:47 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/04 21:30:29 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// should check if port less than 1024 and great than 65535
#include "irc.hpp"

void	error_msg(std::string msg){
	std::perror(msg.c_str());
	exit (1);
}

void	irc_server::setSocketFd(int socket_fd){
	this->socket_fd = socket_fd;
}

void	irc_server::getSocketFd(void){
	return (socket_fd);
}

void	irc_server::init_sockets(char **argv)
{
	int	socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1){
		error_msg("Socket ");
	}
}

void	irc_server::band_sockets(void)
{
	struct sockaddr_in addr_serv;
	std::memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port = htons(portno); 
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	if (band())
}

void	irc_server(char **av)
{
	int	sockfd;
	int	acceptfd;
	int	portno = atoi(av[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		error_msg("Socket ");
	}
	struct sockaddr_in addr_serv;
	std::memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port = htons(portno); 
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sockfd, (struct sockaddr *) &addr_serv, sizeof(addr_serv)) == -1){
		error_msg("Bind: ");
	}
	if (listen(sockfd, NB_QUEUE) == -1){
		error_msg("listen ");
	}
	struct sockaddr_storage addr_client;
	socklen_t clientlen = sizeof(addr_client);
	acceptfd = accept(sockfd, (struct sockaddr *) &addr_client, &clientlen);
	if (acceptfd == -1){
		error_msg("Accept ");
	}
	std::vector<int> vec_fd;
	for(std::vector<int>::iterator it = vec_fd.begin(); it != vec_fd.end(); ++it){
		vec_fd.push_back(sockfd); // fill in the vector with new fd socket each time;	
	}
	struct pollfd poll_[vec_fd.size()];
	std::memset(poll_, 0, sizeof(poll));
	for (int i = 0; poll_[i]; i++){
		poll_[i] = vec_fd[i];
		std::cout << poll_[i] << std::endl;
	}
	while (true){
		char buffer[BUFFER_SIZE] = {0};
    	int reading = recv(acceptfd, buffer, BUFFER_SIZE, 0);
		if (reading == -1) {
			error_msg("recev ");
		}
		else if (reading == 0){
			error_msg("connection closed by client");
		}
		else{
			std::cout << "Here is the message: " << buffer << std::endl;
		}
	}
	// close(acceptfd);
	// close(sockfd);
}

int	main(int ac, char **av){
	if (ac == 3){
		irc_server(av);
	}
}

irc_server::irc_server(){
	
}

irc_server::~irc_server(){
	
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:26:47 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/06 18:14:04 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void irc_server::check_port(char **argv)
{
    int port = std::stoi(argv[1]);
    if (port < 1024 || port > 65535)
        throw std::out_of_range("Port number out of the bounds");
    else {
        this->portno = std::stoi(argv[1]);
        std::cout << this->portno << std::endl;
    }
}

void	irc_server::init_sockets(void)
{
	int	socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_fd == -1){
		std::perror("socket ");
		exit (0);
	}
    int hh = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &hh, sizeof(hh));
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
	this->socket_fd = socket_fd;
}

void	irc_server::bind_sockets(void)
{
	struct sockaddr_in addr_serv;
	std::memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port = htons(this->portno);
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->socket_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) == -1){
		std::perror("bind ");
		exit (0);
	}
}

void	irc_server::listenToIncomingconnection(void)
{
	if (listen(this->socket_fd, SOMAXCONN) == -1){
		std::perror("listen ");
		exit (0);
	}
}

void	irc_server::multipleconnection(void)
{
	struct pollfd fds;
    fds.fd = this->socket_fd;
    fds.events = POLLIN;
	vec_fd.push_back(fds);
}

void irc_server::receivingmessages()
{
    while (true) 
    {
        // for (size_t i = 0; i < vec_fd.size(); i++)
        // {
        //     std::cout << vec_fd[i].fd << std::endl; 
        // }
        int poll_fd = poll(&vec_fd[0], vec_fd.size(), -1);
        if (poll_fd == -1) {
            perror("poll");
            exit(0);
        }
        puts("Im under poll pls help hh theres too much runing");
        for (size_t i = 0; i < vec_fd.size(); i++) 
        {
            if (vec_fd[i].revents == POLLIN)
            {
                if (vec_fd[i].fd == this->socket_fd)
                {
                    struct sockaddr_storage addr_client;
                    socklen_t client_len = sizeof(addr_client);
                    int accept_fd = accept(this->socket_fd, (struct sockaddr *)&addr_client, &client_len);
                    if (accept_fd == -1) 
                    {
                        perror("accept");
                        exit(0);
                    }
                    this->accept_fd = accept_fd;
                    struct pollfd clients_fd;
                    clients_fd.fd = this->accept_fd;
                    clients_fd.events = POLLIN;
                    vec_fd.push_back(clients_fd);
                }
                else
                {
                    char buffer[BUFFER_SIZE] = {0};
                    int reading = recv(vec_fd[i].fd, buffer, BUFFER_SIZE, 0);
                    if (reading == -1) {
                        perror("recv");
                        exit(0);
                    } else if (reading == 0) {
                        std::cerr << "Connection closed by the client" << std::endl;
                        // close (the sockets);
                    } else {
                        std::cout << "Here is the message: " << buffer << std::endl;
                    }
                }
            }
        }
    }
}

void	irc_server::createServer(void)
{
	irc_server	obj;
	
	obj.init_sockets();
	obj.bind_sockets();
	obj.listenToIncomingconnection();
	obj.multipleconnection();
	obj.receivingmessages();
}

int	irc_server::getSocketFd(void){
	return (this->socket_fd);
}

irc_server::irc_server(){
	
}

irc_server::~irc_server(){
	
}

void	irc_server::setSocketFd(int socket_fd){
	this->socket_fd = socket_fd;
}
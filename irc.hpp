/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 20:07:31 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/09 18:11:35 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cstdio>
#include <poll.h>
#include <vector>
#include <string>
#include <stdexcept>

#define BUFFER_SIZE 1024
#define	EXIT_FAILURE 1

class irc_server{
	private:
		int	socket_fd;
		int	portno;
		int	passwd;
		int	accept_fd;
		int	poll_fds;
		std::vector<pollfd> vec_fd;
    public:
		void setSocketFd(int socket_fd);
		int	getSocketFd(void);
        irc_server();
        ~irc_server();
        void init_sockets(void);
		void bind_sockets(void);
		void listenToIncomingconnection(void);
		void AcceptToIncomingconnection(void);
		void multipleconnection(void);
		void check_port(char **argv);
		void createServer(void);
		void ReusableSocket(void);
		void non_blocking(void);
		// void recievingmessage(void);
};

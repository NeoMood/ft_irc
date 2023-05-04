/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 20:07:31 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/04 21:17:58 by yamzil           ###   ########.fr       */
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

#define NB_QUEUE 5
#define BUFFER_SIZE 1024

class irc_server{
	private:
		int	socket_fd;
    public:
        irc_server();
        ~irc_server();
        void init_sockets(char **argv);
		void band_sockets(void);
		void setSocketFd(int socket_fd);
		int	getSocketFd(void);
};
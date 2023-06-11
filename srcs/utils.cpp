/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayoubaqlzim <ayoubaqlzim@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 01:29:51 by yamzil            #+#    #+#             */
/*   Updated: 2023/06/11 22:15:19 by ayoubaqlzim      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Reply.hpp"

void irc_server::check_port(char **argv)
{
	std::string port = argv[1];
	std::string password = argv[2];
    if (port < "1024" || port > "65535")
        throw std::out_of_range("Port number out of the bounds");
    this->portno = std::stoi(port);
    this->passwd = password;
}

void    irc_server::send_message(int fd, std::string message){
    write (fd, message.c_str(), message.length());
}

bool    irc_server::check_param(const char *nickname, Client &client){
    
	for (size_t i = 0; nickname[i]; i++){
		if (nickname[i] == ',' || nickname[i] == '*' || nickname[i] == '?' || nickname[i] == '@' || nickname[i] == '.'){
			send_message(client.getFdNumber(), ERR_ERRONEUSNICKNAME(std::string(nickname), client.getUserName(), host));
            return (false);
		}
		else if (std::strlen(nickname) >= 9){
			send_message(client.getFdNumber(), ERR_ERRONEUSNICKNAME(std::string(nickname), client.getUserName(), host));
			return (false);
		}
	}
	return (true);
}

void	irc_server::welcome_message(int fd, std::string message){ 
	write(fd, message.c_str(), message.length());
}

void	irc_server::get_date(void){
	
	time_t	t;
	
	struct tm* date;
	time(&t);
	date = localtime(&t);
	send_message(0, RPL_CREATED(asctime(date)));
}

bool irc_server::validateValue(std::string value) const {
	std::size_t i = 0;
	for (; i < value.length(); i++) {
		if (value[i] < '0' || value[i] > '9')
			return false;
	}
    return true;
}

bool irc_server::isLeap(int year) const {
    return (((year % 4 == 0) && 
            (year % 100 != 0)) ||
            (year % 400 == 0));
}
  

bool irc_server::validateDate(std::string date, std::string dlm) const {
    if (!date.length()) {
        return false;
    }
    std::size_t pos = date.find_first_of(dlm);
    if (pos != std::string::npos) {
        std::string op = date.substr(0, pos);
        if (!validateValue(op) || !op.length()) {
            return false;
        }
        int year = std::atoi(op.c_str());
        date.erase(0, pos + 1);
        pos = date.find(dlm);
        if (pos != std::string::npos) {
            op = date.substr(0, pos);
            if (!validateValue(op)) {
                return false;
            }
            int m = std::atoi(op.c_str());
            date.erase(0, pos + 1);
            if (!validateValue(date)) {
                return false;
            }
            int d = std::atoi(date.c_str());
            if (m < 1 || m > 12)
                return false;
            if (d < 1 || d > 31)
                return false;
            if (m == 2) {
                if (isLeap(year))
                    return (d <= 29);
                else
                    return (d <= 28);
            }
            if (m == 4 || m == 6 || m == 9 || m == 11)
                return (d <= 30);
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

bool irc_server::isDir(const char* path) {
    struct stat stats;
    stat(path, &stats);
    return S_ISDIR(stats.st_mode);
}
bool irc_server::isFile(const char* path) {
    struct stat stats;
    stat(path, &stats);
    return S_ISREG(stats.st_mode);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:56:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/17 17:51:23 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

// Client::Client(std::string Name, std::string ip_adress, int fd_number)
// {
//     this->Name = Name;
//     this->ip_adress = ip_adress;
//     this->fd_number = fd_number;
// }

Client::Client(){
    this->isregistred = false;
    this->nicknamesited = false;
}

Client::~Client(){
    
}

void    Client::setName(std::string _Name){
    this->Name = _Name;
}

void    Client::setip_adress(std::string _ip_adress){
    this->ip_adress = _ip_adress;
}

void    Client:: setfd_number(int _fd_number){
    this->fd_number = _fd_number;
}

std::string Client::getName(void){
    return(this->Name);
}

void	Client::setisregistred(bool _isregistred){
   this->isregistred = _isregistred;
}

void	Client::setnicknamesited(bool _nicknamesited){
   this->nicknamesited = _nicknamesited;
}

bool    Client::getnicknamesited(){
    return(this->nicknamesited);
}

bool    Client::getisregistred(){
    return(this->isregistred);
}

std::string Client::getip_adress(void){
    return(this->ip_adress);
}

int Client::getfd_number(void){
    return(this->fd_number);
}
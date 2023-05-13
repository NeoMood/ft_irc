/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:56:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/12 16:03:43 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client(std::string Name, std::string ip_adress, int fd_number)
{
    this->Name = Name;
    this->ip_adress = ip_adress;
    this->fd_number = fd_number;
}

Client::Client(){
    
}

Client::~Client(){
    
}

void    Client::setName(std::string Name){
    this->Name = Name;
}

void    Client::setip_adress(std::string ip_adress){
    this->ip_adress = ip_adress;
}

void    Client:: setfd_number(int fd_number){
    this->fd_number = fd_number;
}

std::string Client::getName(void){
    return(this->Name);
}

std::string Client::getip_adress(void){
    return(this->ip_adress);
}

int Client::getfd_number(void){
    return(this->fd_number);
}
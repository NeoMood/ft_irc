/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:56:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/23 11:22:46 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(){
    this->PasswordApproved = false;
    this->NicknameSited = false;
    this->UsernameSited = false;
}

Client::~Client(){
}

// Setters

void    Client::setNickname(std::string _Name){
    this->Nickname = _Name;
}

void    Client::setfd_number(int _fd_number){
    this->FdNumber = _fd_number;
}

void	Client::setPasswordApproved(bool _Password){
   this->PasswordApproved = _Password;
}

void	Client::setNicknameSited(bool _Nicknamesited){
   this->NicknameSited = _Nicknamesited;
}

std::string Client::getNickname(void){
    return(this->Nickname);
}

void    Client::setIPAddress(std::string _IPAdresss){
    this->IPAddress = _IPAdresss;
}

void    Client::setUserName(std::string _user){
    this->UserName = _user;
}

void    Client::setMode(std::string _mode){
    this->mode = _mode;
}

void    Client::setHostname(std::string _hostname){
    this->hostname = _hostname;
}

void    Client::setRealName(std::string _realname){
    this->RealName = _realname;
}

void    Client::setUsernameSited(bool _Usernamesited){
    this->UsernameSited = _Usernamesited;
}

// Getters

bool    Client::getNickNameSited(){
    return(this->NicknameSited);
}

bool    Client::getPasswordApproved(){
    return(this->PasswordApproved);
}

int Client::getFdNumber(void){
    return(this->FdNumber);
}

std::string Client::getHostname(void){
    return (this->hostname);
}

std::string Client::getRealName(void){
    return (this->RealName);
}

std::string Client::getUserName(void){
    return(this->UserName);
}

std::string Client::getMode(void){
    return(this->mode);
}

bool    Client::getUserNameSited(void){
    return (this->UsernameSited);
}
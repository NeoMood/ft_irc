/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgmira <sgmira@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 00:56:36 by yamzil            #+#    #+#             */
/*   Updated: 2023/06/13 17:14:59 by sgmira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(): channel_mode(){
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

void Client::set_disguised(bool disguised){
    this->disguised = disguised;
}

void Client::set_invisible(bool invisible){
    this->invisible = invisible;
}

void Client::set_wallops(bool wallops){
    this->wallops = wallops;
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

void Client::decrementChannelCount() {
    if (_channels_count != 0)
        _channels_count -= 1;
}

int Client::getChannelCount() const {
    return _channels_count;
}

void Client::incrementChannelCount() {
    _channels_count += 1;
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

void    Client::setChannelMode(channel_mode_t _mode) {
    channel_mode.push_back(_mode);
}

void    Client::setUserMode(user_mode_t _mode) {
    user_mode.push_back(_mode);
}

bool Client::get_invisible(void){
    return (this->invisible);
}

bool Client::get_wallops(void){
    return (this->wallops);
}

std::vector<user_mode_t> Client::get_user_mode(void)
{
    return (this->user_mode);
}
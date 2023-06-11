#include "../includes/File.hpp"

File::File() {}

File::File(std::string name, std::string key, std::string filepath, std::string sender, std::string reciever) {
    this->_name = name;
    this->_key = key;
    this->_filepath = filepath;
    this->_sender = sender;
    this->_reciever = reciever;
}

std::string File::getName() const {
    return this->_name;
}

std::string File::getKey() const {
    return this->_key;
}

std::string File::getFilePath() const {
    return this->_filepath;
}

std::string File::getSender() const {
    return this->_sender;
}

std::string File::getReciever() const {
    return this->_reciever;
}

File::~File() {}
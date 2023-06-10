#pragma once

#include "../includes/Client.hpp"

class File {
    private:
        std::string _name;
        std::string _filepath;
        std::string _sender;
        std::string _reciever;
    public:
        File();
        File(std::string name, std::string filepath, std::string sender, std::string reciever);
        std::string getName() const;
        std::string getFilePath() const;
        std::string getSender() const;
        std::string getReciever() const;
        ~File();
};

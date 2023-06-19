#pragma once

#include <string>
#include <iostream>
#include <stdio.h>
#include "Reply.hpp"
#include "File.hpp"


class FileTransfer {
    private:
        FileTransfer();
        static FileTransfer* _instance;
        void replace_special_chars(std::string& str);
        std::map<std::string, File>::iterator findFile(std::map<std::string, File>& files, std::string name);
        std::map<std::string, File>::iterator findFileBySender(std::map<std::string, File>& files, std::string name, std::string sender);
        static int ID;
        static int UID;
    public:
        static FileTransfer& getInstance();
        bool uploadFile(std::string sender, std::string reciever, std::string path, std::map<std::string, File>& files);
        std::string downloadFile(std::string name, std::string sender, std::map<std::string, File>& files);
        std::string listFiles(std::string msg, std::string sender, std::string receiver, std::map<std::string, File>& files);
        ~FileTransfer();
};

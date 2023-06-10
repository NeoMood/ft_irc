#pragma once

#include <string>
#include <iostream>
#include "File.hpp"


class FileTransfer {
    private:
        FileTransfer();
        static FileTransfer* _instance;
    public:
        static FileTransfer& getInstance();
        void uploadFile(std::string key, Client& client, std::string sender, std::string path, std::map<std::string, File>& files);
        void readFile(std::string name, std::map<std::string, File>& files);
        void downloadFile(std::string name, std::map<std::string, File>& files);
        ~FileTransfer();
};

#include "../includes/FileTransfer.hpp"

FileTransfer *FileTransfer::_instance = nullptr;

FileTransfer::FileTransfer() {}

FileTransfer &FileTransfer::getInstance() {
    if (!_instance)
        _instance = new FileTransfer();
    return *_instance;
}

void FileTransfer::uploadFile(std::string key, Client& client, std::string sender, std::string path, std::map<std::string, File>& files) {
    std::size_t pos = path.find_last_of("/");
    std::string name = path;
    if (pos != std::string::npos) {
        name = path.substr(pos + 1);
    }
    File file = File(name, path, client.getNickname(), sender);
    files.insert(std::pair<std::string, File>(key, file));
    // std::string name = path.substr();
}

void FileTransfer::readFile(std::string name, std::map<std::string, File>& files) {
    //
}
void FileTransfer::downloadFile(std::string name, std::map<std::string, File>& files) {
    //
}

FileTransfer::~FileTransfer() {
    if (!_instance) {
        delete _instance;
        _instance = nullptr;
    }
}
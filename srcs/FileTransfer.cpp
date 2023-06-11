#include "../includes/FileTransfer.hpp"

FileTransfer *FileTransfer::_instance = nullptr;
int FileTransfer::ID = 0;
int FileTransfer::UID = 0;

FileTransfer::FileTransfer() {}

void FileTransfer::replace_special_chars(std::string& str) {
    size_t pos = 0;
    if ((pos = str.find_first_of(" !\"#$%&'()*+,:;<=>?@[\\]^`{|}~")) != std::string::npos) {
        str.replace(pos, 1, "_");
    }
}

FileTransfer &FileTransfer::getInstance() {
    if (!_instance)
        _instance = new FileTransfer();
    return *_instance;
}

std::map<std::string, File>::iterator FileTransfer::findFile(std::map<std::string, File>& files, std::string name) {
    for (std::map<std::string, File>::iterator it = files.begin(); it != files.end(); it++) {
        if (it->second.getName() == name)
            return it;
    }
    return files.end();
}

std::map<std::string, File>::iterator FileTransfer::findFileBySender(std::map<std::string, File>& files, std::string name, std::string sender) {
    for (std::map<std::string, File>::iterator it = files.begin(); it != files.end(); it++) {
        if (it->second.getName() == name && it->second.getSender() == sender)
            return it;
    }
    return files.end();
}


bool FileTransfer::uploadFile(std::string sender, std::string reciever, std::string path, std::map<std::string, File>& files) {
    std::size_t pos = path.find_last_of("/");
    std::string fmt;
    std::string name = path;
    std::string cmd;
    char buff[128];
    std::memset(buff, 0, 128);
    FILE *fptr;
    std::string content;
    std::string key;
    if (pos != std::string::npos) {
        name = path.substr(pos + 1);
    }
    pos = name.find(".");
    if (pos != std::string::npos) {
        fmt = name.substr(pos);
        name = name.substr(0, pos);
    }
    std::map<std::string, File>::iterator it = findFileBySender(files, name, sender);
    if (it != files.end()) {
        std::stringstream ss;
        ss << ID;
        name = name + "_" + ss.str() + fmt;
        cmd = "curl --upload-file " + path + " https://free.keep.sh/" + name;
        fptr = popen(cmd.c_str(), "r");
        if (!fptr) {
            std::cout << "failed popen\n";
            return false;
        } else {
            while (fgets(buff, 128, fptr)) {
                content += buff;
            }
            pclose(fptr);
            ID += 1;
            std::cout << "Content: " << content << std::endl;
            pos = content.find_last_of("/");
            if (pos != std::string::npos) {
                content.erase(pos);
                pos = content.find_last_of("/");
                if (pos != std::string::npos) {
                    key = content.substr(pos + 1);
                    std::cout << "Key: " << key << std::endl;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    } else {
        name += fmt;
        cmd = "curl --upload-file " + path + " https://free.keep.sh/" + name;
        fptr = popen(cmd.c_str(), "r");
        if (!fptr) {
            std::cout << "failed popen\n";
            return false;
        } else {
            while (fgets(buff, 128, fptr)) {
                content += buff;
            }
            pclose(fptr);
            std::cout << "Content: " << content << std::endl;
            pos = content.find_last_of("/");
            if (pos != std::string::npos) {
                content.erase(pos);
                std::cout << "after Content: " << content << std::endl;
                pos = content.find_last_of("/");
                if (pos != std::string::npos) {
                    key = content.substr(pos + 1);
                    std::cout << "Key: " << key << std::endl;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    File file = File(name, key, path, sender, reciever);
    files.insert(std::pair<std::string, File>(key, file));
    return true;
}


std::string FileTransfer::downloadFile(std::string name, std::string sender, std::map<std::string, File>& files) {
    std::map<std::string, File>::iterator it = findFileBySender(files, name, sender);
    std::string downloadedfile;
    if (it != files.end()) {
        std::string cmd;
        std::string fmt;
        std::string filename = name;
        std::stringstream ss;
        ss << UID;
        std::size_t pos = name.find(".");
        if (pos != std::string::npos) {
            fmt = name.substr(pos);
            name = name.substr(0, pos);
        }
        downloadedfile = sender + "_" + name + "_" + ss.str() + fmt;
        cmd = "curl -L  https://free.keep.sh/" + it->first + "/" + filename + " > " + downloadedfile;
        FILE *fptr = popen(cmd.c_str(), "r");
        if (!fptr) {
            return "";
        }
        pclose(fptr);
    } else {
        return "";
    }
    return downloadedfile;
}

std::string FileTransfer::listFiles(std::string msg, std::string sender, std::string receiver, std::map<std::string, File>& files) {
    std::string reply;
    if (sender.empty()) {
        for (std::map<std::string, File>::iterator it = files.begin(); it != files.end(); it++) {
            if (it->second.getReciever() == receiver)
                reply += msg + RPL_LISTFILE(it->second.getSender(), it->second.getName());
        }
    } else {
        for (std::map<std::string, File>::iterator it = files.begin(); it != files.end(); it++) {
            if (it->second.getSender() == sender && it->second.getReciever() == receiver)
                reply += msg + RPL_LISTFILE(it->second.getSender(), it->second.getName());
        }
    }
    reply += msg + RPL_LISTFILEEND(receiver);
    return reply;
}

FileTransfer::~FileTransfer() {
    if (!_instance) {
        delete _instance;
        _instance = nullptr;
    }
}
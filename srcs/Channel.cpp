#include "../includes/Channel.hpp"

Channel::Channel(std::string name, Client& _operator): __name(name), __topic(), __key(""), mask(), support_modes(true), __owner(_operator), __users(), __banned_users(), __invited_users(), __online_users(1) {
    mask += name[0];
    __is_invite_only = true; // change to false
    if (mask == "+") {
        support_modes = false;
    }
    if (mask == "!") {
        __owner.setChannelMode(o);
    }
}

bool Channel::isInviteOnly() const {
    return this->__is_invite_only;
}

void Channel::setInviteOnly(bool inviteOnly) {
    this->__is_invite_only = inviteOnly;
}

bool Channel::is_already_join(Client& client) {
    if (this->__users.find(client.getNickname()) != this->__users.end()) {
        return true;
    }
    return false;
}

int Channel::invite_user(Client& client) {
    Logger logger = Logger::getLogger();
    logger.log(DEBUG, "Check if the user banned or not");
    for (std::map<std::string, Client&>::iterator it = __banned_users.begin(); it != __banned_users.end(); it++) {
        if (client.getNickname() == it->first) {
            return 0;
        }
    }
    if (__is_invite_only) {
        for (std::map<std::string, Client&>::iterator it = __invited_users.begin(); it != __invited_users.end(); it++) {
            if (client.getNickname() == it->first) {
                return 1;
            }
        }
        __invited_users.insert(std::pair<std::string, Client&>(client.getNickname(), client));
    }
    logger.log(DEBUG, "size of __invited_users is " + std::to_string(__invited_users.size()));
    return 1;
}

int Channel::join_user(Client& client) {
    Logger logger = Logger::getLogger();
    bool __has_ben_invited = false;
    logger.log(DEBUG, "Check if the user banned or not");
    for (std::map<std::string, Client&>::iterator it = __banned_users.begin(); it != __banned_users.end(); it++) {
        if (client.getNickname() == it->first) {
            return 0;
        }
    }
    if (__is_invite_only) {
        for (std::map<std::string, Client&>::iterator it = __invited_users.begin(); it != __invited_users.end(); it++) {
            if (client.getNickname() == it->first) {
                __has_ben_invited = true;
                break ;
            }
        }
    }
    if (((__is_invite_only && __has_ben_invited) || !__is_invite_only) && !is_already_join(client) && client.getNickname() != __owner.getNickname()) {
        this->__users.insert(std::pair<std::string, Client&>(client.getNickname(), client));
        __online_users++;
    }
    logger.log(DEBUG, "size of __users is " + std::to_string(__users.size()));
    return 1;
}

std::map<std::string, Client&>  Channel::getUsers() {
    return this->__users;
}

std::map<std::string, Client&>::iterator Channel::getUser(std::string nickname) {
    // if (this->__users.find(nickname) != this->__users.end()) {
    //     return this->__users.find(nickname);
    // }
    for (std::map<std::string, Client&>::iterator it = __users.begin(); it != __users.end(); it++) {
        // std::cout << "nickname: " << it->first << " name " << nickname << std::endl;
        if (it->first == nickname) {
            return it;
        }
    }
    return this->__users.end();
}

int Channel::remove_user(Client& client) {
    this->__users.erase(client.getNickname());
    return 0;
}
int Channel::ban_user(Client& client) {
    if (this->__users.find(client.getNickname()) != this->__users.end()) {
        this->__users.erase(client.getNickname());
        this->__banned_users.insert(std::pair<std::string, Client&>(client.getNickname(), client));
    }
    return 0;
}

int Channel::add_operator(Client& client) {
    (void) client;
    return 0;
}

int Channel::unban_user(Client& client) {
    (void) client;
    return 0;
}

Client Channel::getCreator() const {
    return this->__owner;
}

std::string Channel::getChannelName() const {
    return this->__name;
}
std::string Channel::getChannelKey() const {
    return this->__key;
}

bool Channel::hasKey() const {
    return this->__key != "";
}

void Channel::setChannelKey(std::string key) {
    this->__key = key;
}

Channel::~Channel() {
}

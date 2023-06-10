#include "../includes/Channel.hpp"

Channel::Channel(std::string name, Client& _operator): __name(name), __topic(), __key(""), mask(), support_modes(true), __owner(_operator), __users(), __banned_users(), __invited_users(), __online_users(1), __users_limit(-1), support_topic(false) {
    mask += name[0];
    __owner.incrementChannelCount();
    __is_invite_only = false;
    if (mask == "+") {
        support_modes = false;
    }
    if (mask == "!") {
        __owner.setChannelMode(o);
    }
}

bool Channel::isAnOperatorOrOwner(Client& clinet) {
    if (!clinet.getNickname().compare(0, clinet.getNickname().length(), __owner.getNickname())) {
        return true;
    } else {
        for (std::map<std::string, Client&>::iterator it = __operators.begin(); it != __operators.end(); it++) {
            if (clinet.getNickname() == it->first) {
                return true;
            }
        }
    }
    return false;
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

void Channel::buildJoinChannelRes(Client& client) {
    std::string message = RPL_TOPIC(client.getHostname(), client.getNickname(), __name);
    std::string users = __owner.getNickname() + " ";
    for (std::map<std::string, Client&>::iterator it = __users.begin(); it != __users.end(); it++) {
        users += it->first + " ";
    }
    users = users.substr(0, users.find_last_of(" "));
    message += RPL_NAMREPLY(client.getHostname(), client.getNickname(), __name, users);
    message += RPL_ENDOFNAMES(__name);
    write(client.getFdNumber(), message.c_str(), message.length());
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
    if (__is_invite_only && !__has_ben_invited) {
        std::string message = ERR_INVITEONLYCHAN(client.getNickname(), __name);
        write (client.getFdNumber(), message.c_str(), message.length());
    }
    if (((__is_invite_only && __has_ben_invited) || !__is_invite_only) && !is_already_join(client) && client.getNickname() != __owner.getNickname()) {
        client.incrementChannelCount();
        this->__users.insert(std::pair<std::string, Client&>(client.getNickname(), client));
        sendToAllUsers(client, RPL_CHANNEL(client.getNickname(), client.getUserName(), client.getHostname(), __name), true);
        buildJoinChannelRes(client);
        __online_users++;
    }
    logger.log(DEBUG, "size of __users is " + std::to_string(__users.size()));
    return 1;
}

void Channel::sendToAllUsers(Client& client, std::string message, bool sendToOwner) {
    if (sendToOwner && client.getNickname() != __owner.getNickname())
        write(__owner.getFdNumber(), message.c_str(), message.length());
    for (std::map<std::string, Client&>::iterator it = __users.begin(); it != __users.end(); it++) {
        if (client.getNickname() != it->first)
            write(it->second.getFdNumber(), message.c_str(), message.length());
    }
}

std::map<std::string, Client&>  Channel::getUsers() {
    return this->__users;
}

bool Channel::hasUser(std::string nickname) {
    if (__owner.getNickname() == nickname) {
        return true;
    }
    for (std::map<std::string, Client&>::iterator it = __users.begin(); it != __users.end(); it++) {
        // std::cout << "nickname: " << it->first << " name " << nickname << std::endl;
        if (it->first == nickname) {
            return true;
        }
    }
    return false;
}

int Channel::remove_user(Client& client) {
    __operators.erase(client.getNickname());
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

void Channel::add_operator(Client& client) {
    this->__operators.insert(std::pair<std::string, Client&>(client.getNickname(), client));
}

void Channel::remove_operator(Client& client) {
    this->__operators.erase(client.getNickname());
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

void Channel::setChannelLimit(int lmt) {
    this->__users_limit = lmt;
}

void Channel::setSupportTopic(bool support) {
    support_topic = support;
}
bool Channel::getSupportTopic() const {
    return this->support_topic;
}

int Channel::getUserLimit() const {
    return this->__users_limit;
}

int Channel::getUsersTotal() {
    return this->__users.size() + 1;
}

std::string Channel::getTopic() const {
    return this->__topic;
}

Channel::~Channel() {
}

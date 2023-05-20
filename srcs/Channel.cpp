#include "../includes/Channel.hpp"

Channel::Channel(std::string name, Client* _operator): __name(name), __key(), __owner(_operator), __users(), __banned_users(), __online_users(1) {}

int Channel::join(Client *client) {

    return 0;
}


Channel::~Channel() {
}

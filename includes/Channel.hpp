#pragma once

#include <map>
#include <vector>
#include <stdexcept>
#include <string>
#include "client.hpp"

// Numeric Replies:

// # define ERR_NEEDMOREPARAMS
// # define ERR_BANNEDFROMCHAN
// # define ERR_INVITEONLYCHAN
// # define ERR_BADCHANNELKEY
// # define ERR_CHANNELISFULL
// # define ERR_BADCHANMASK
// # define ERR_NOSUCHCHANNEL
// # define ERR_TOOMANYCHANNELS
// # define ERR_TOOMANYTARGETS
// # define ERR_UNAVAILRESOURCE

// Command: JOIN
//    Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] )
//                / "0"
// JOIN, NOTICE, PRIVMSG -> file, MODE, bot
class Channel {
    private:
        std::string                 __name;
        std::string                 __key;
        Client*                     __owner;
        std::map<int, Client*>        __users;
        std::map<int, Client*>        __banned_users;
        std::map<int, Client*>        __operators;
        int                         __online_users;
    public:
        Channel(std::string name, Client* _operator);
        int join(Client *client);
        int remove_user(Client *client);
        int ban_user(Client *client);
        ~Channel();
};

// std::vector<Channel> *g_Channels;
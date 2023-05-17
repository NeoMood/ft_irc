#pragma once

#include <map>
#include <vector>
#include <stdexcept>
#include <string>
#include "../client.hpp"

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

class Channel {
    private:
        std::string __name;
        std::string __key;
        Client      *__users;
        Client      *__banned_users;
        int         __online_users;
    public:
        Channel();
};
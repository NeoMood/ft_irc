#pragma once

#include <map>
#include <vector>
#include <stdexcept>
#include <string>
#include "Client.hpp"
#include "Logger.hpp"
#include "Reply.hpp"


class Channel {
    private:
        std::string                 __name;
        std::string                 __topic;
        std::string                 __key;
        std::string                 mask;
        bool                        support_modes;
        bool                        __is_invite_only;
        Client&                     __owner;
        std::map<std::string, Client&>        __users;
        std::map<std::string, Client&>        __banned_users;
        std::map<std::string, Client&>        __operators;
        std::map<std::string, Client&>        __invited_users;
        int                         __online_users;
        int                         __users_limit;
        bool                        support_topic;
        bool                        __has_owner;
    public:
        Channel(std::string name, Client& _operator);
        bool isInviteOnly() const;
        void setInviteOnly(bool inviteOnly);
        void setChannelKey(std::string key);
        void setChannelLimit(int lmt);
        void setChannelTopic(std::string topic);
        bool is_already_join(Client& client);
        bool isAnOperatorOrOwner(Client& clinet);
        int join_user(std::string msg, std::string host, Client& client);
        int invite_user(Client& client);
        int remove_user(Client& client);
        int ban_user(Client& client);
        void add_operator(Client& client);
        void remove_operator(Client& client);
        int unban_user(Client& client);
        bool hasUser(std::string nickname);
        std::map<std::string, Client&>  getUsers();
        std::map<std::string, Client&>  getOperators();
        std::string getChannelName() const;
        std::string getChannelKey() const;
        bool hasKey() const;
        Client getCreator() const;
        int getUserLimit() const;
        int getUsersTotal();
        void sendToAllUsers(Client& client, std::string message, bool sendToOwner);
        void setSupportTopic(bool support);
        bool getSupportTopic() const;
        std::string getChannelTopic() const;
        void buildJoinChannelRes(std::string msg, Client& client);
        bool hasOwner() const;
        ~Channel();
};

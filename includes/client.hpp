#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cstdio>
#include <poll.h>
#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#include <map>
#include <sstream>

#pragma	once

typedef enum {
    i = 0,
    t = 1,
    k = 2,
    o = 3,
    l = 4,

} channel_mode_t;

typedef enum
{
    u_i = 0, //Invisible
    u_w = 1, //Receiving wallops
    u_o = 2, //Operator
    u_s = 3, //Secure connection
    u_x = 4, //Cloaking disguises the user's actual IP address, replacing it with a generic or obfuscated address
    u_D = 5, //allows the user to ignore all channel messages while still being connected to the channel
    u_r = 6,
} user_mode_t;

class Client{
    private:
        bool    PasswordApproved;
        bool    NicknameSited;
        bool    UsernameSited;
        std::string Nickname;
        std::string	IPAddress;
        std::string UserName;
        std::string mode;
        std::string hostname;
        std::string RealName;
        int FdNumber;
        std::vector<channel_mode_t> channel_mode;
        int        _channels_count;
        bool       _server_operator;
        time_t joining_time;
        std::vector<user_mode_t> user_mode;
        bool disguised;
        bool invisible;
        bool wallops;
        bool rest_conx;
    public:
        Client();
        ~Client();
    // Setters
        void	setNicknameSited(bool _Nicknamesited);
        void	setUsernameSited(bool _Usernamesited);
		void	setIPAddress(std::string _IPAdresss);
        void    setHostname(std::string _hostname);
        void    setRealName(std::string _realname);
		void	setPasswordApproved(bool _Password);
        void    setUserName(std::string _user);
        void	setfd_number(int fd_number);
        void    setMode(std::string _mode);
        void	setNickname(std::string Nickname);
        void    setChannelMode(channel_mode_t _mode);
    // Getters
        bool		getNickNameSited(void);
        bool		getUserNameSited(void);
        int			getFdNumber(void);
        bool		getPasswordApproved(void);
        std::string	getNickname(void);
        std::string	getHostname(void);
        std::string	getRealName(void);
        std::string	getUserName(void);
        std::string	getMode(void);
        std::string MessageFormat;
        void incrementChannelCount();
        void decrementChannelCount();
        int getChannelCount() const;
        void setServerOper(bool isOper);
        bool isServerOper() const;
        void setJoiningTime(time_t time);
        time_t getJoiningTime() const;
        std::string getJoiningTimeAsString();
        void setUserMode(user_mode_t _mode);
        void set_disguised(bool disguised);
        void set_invisible(bool invisible);
        void set_wallops(bool wallops);
        void set_rest_conx(bool rest_conx);
        bool get_invisible(void);
        bool get_wallops(void);
        bool get_rest_conx(void);
        std::vector<user_mode_t> get_user_mode(void);
};
#pragma once

# include <string>
# include <time.h>
# include <iostream>
# include <iomanip>

# define RESET "\e[0m"
# define GREEN "\e[1;32m"
# define RED "\e[1;91m"
# define CYAN "\e[1;36m"
# define YELLOW "\e[1;33m"
# define PURPLE "\e[1;35m"
# define BLUE "\e[1;34m"


/* Log levels */
typedef enum {
    QUIET    = 00,
    CRITICAL = 10,
    ERROR    = 20,
    WARN     = 30,
    INFO     = 40,
    DEBUG    = 50,
    TRACE    = 60,
    UNKNOWN = -1,
} loglevel_t;

class Logger {
    public:
        ~Logger();
        static Logger& getLogger();
        void log(loglevel_t level, std::string fmt);
    private:
        void __log(loglevel_t level, std::string data);
        static Logger* _logger;
        Logger();
};

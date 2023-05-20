#include "../includes/Logger.hpp"

Logger *Logger::_logger = nullptr;

Logger &Logger::getLogger()
{
    if (!_logger)
    {
        _logger = new Logger();
    }
    return *_logger;
}

Logger::Logger() {}

void Logger::__log(loglevel_t level, std::string data)
{
    time_t t;
    char buf[80];
    std::string levelstr;

    t = time(NULL);
    strftime(buf, 80, "%Y-%m-%d %H:%M:%S", localtime(&t));

    switch (level)
    {
    case CRITICAL:
        levelstr = "CRITIC";
        break;
    case ERROR:
        levelstr = "ERROR";
        break;
    case WARN:
        levelstr = "WARN";
        break;
    case INFO:
        levelstr = "INFO";
        break;
    case DEBUG:
        levelstr = "DEBUG";
        break;
    case TRACE:
        levelstr = "TRACE";
        break;
    default:
        levelstr = "UNKNOWN";
        break;
    }

    std::cout << "[" << YELLOW << buf << RESET << "]" << std::setw(6) << "[" << GREEN << levelstr << RESET << "] " << BLUE << data << RESET << std::endl;
}

void Logger::log(loglevel_t level, std::string str)
{
    // va_list arg;
    // size_t size = fmt.length();
    // char *str = (char *)malloc(fmt.length());
    // memmove(str, fmt.c_str(), size);
    // va_start(arg, fmt);
    __log(level, str);
    // free(str);
    // va_end(arg);
}

Logger::~Logger()
{
    if (!_logger)
    {
        delete _logger;
        _logger = nullptr;
    }
}
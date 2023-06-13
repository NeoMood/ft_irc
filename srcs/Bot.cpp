#include "../includes/Bot.hpp"

Bot *Bot::_instance = nullptr;

// client_id: u-s4t2ud-0ffa9ea2d9a57c43add851af627d18c9bad19d543662eb62b6291859abe9e921
// secret_id: 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

Bot::Bot() {
    if (!std::getenv("CLIENT_ID") || !std::getenv("SECRET_ID")) {
        _valid = false;
    } else {
        cliend_id = std::string(std::getenv("CLIENT_ID"));
        secret_id = std::string(std::getenv("SECRET_ID"));
        _valid = true;
        if (prelogin() == FILE_CORRUPTED || isExpired()) {
            std::system("rm ./login.json");
            prelogin();
        }
    }
}

bool Bot::isValid() const {
    return _valid;
}

int Bot::prelogin() {
    std::ifstream in("./login.json");
    int ret = 0;
    if (in.is_open()) {
        std::string line;
        std::getline(in, line, '\0');
        ret = parseToken(line);
        in.close();
    } else {
        std::string param = "curl -Ss -X POST --data \"grant_type=client_credentials&client_id=" + cliend_id + "&client_secret=" + secret_id + "\" https://api.intra.42.fr/oauth/token > ./login.json";
        std::system(param.c_str());
        std::ifstream is("./login.json");
        if (is.is_open()) {
            std::string line;
            std::getline(is, line, '\0');
            ret = parseToken(line);
            is.close();
        }
    }
    return ret;
}

int Bot::parseToken(std::string line) {
    std::string dlm = "\"access_token\"";
    size_t pos = line.find(dlm);
    if (pos != std::string::npos) {
        line.erase(0, pos + dlm.length() + 2);
        size_t idx = line.find_first_of("\",");
        if (idx != std::string::npos) {
            token = line.substr(0, idx);
            line.erase(0, idx + 2);
            dlm = "\"created_at\"";
            idx = line.find(dlm);
            if (idx != std::string::npos) {
                line.erase(0, idx + dlm.length() + 1);
                pos = line.find("}");
                if (pos != std::string::npos) {
                    created_date = std::stod(line.substr(0, pos));
                } else {
                    return FILE_CORRUPTED;
                }
            } else {
                return FILE_CORRUPTED;
            }
        } else {
            return FILE_CORRUPTED;
        }
    } else {
        return FILE_CORRUPTED;
    }
    return 0;
}

bool Bot::isExpired() {
    std::fstream in("./login.json");
    if (in.is_open()) {
        in.close();
    }
    else
        return true;
    double diff = time(0) - created_date;
    if (diff >= 7200)
        return true;
    return false;
}

Bot &Bot::getBot() {
    if (!_instance)
        _instance = new Bot();
    return *_instance;
}


std::vector<std::string> Bot::getTimeOfNow() {
    std::vector<std::string> vec;
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    char buff[80];
    strftime(buff, sizeof(buff), "%Y-%m-%d", &tstruct);
    std::string date = buff;
    size_t pos = date.find_first_of("-");
    vec.push_back(date.substr(0, pos));
    date.erase(0, pos + 1);
    pos = date.find_first_of("-");
    vec.push_back(date.substr(0, pos));
    date.erase(0, pos + 1);
    vec.push_back(date.substr(0, pos));
    return vec;
}


std::string Bot::logtime(std::string login, std::string begin, std::string end) {
    std::cout << "login: " << login << "\n";
    std::cout << "begin: " << begin << "\n";
    std::cout << "end: " << end << "\n";
    if (isExpired()) {
        std::system("rm ./login.json");
        prelogin();
    }
    std::stringstream ss;
    std::string query = "";
    std::vector<std::string> date = getTimeOfNow();
    std::string now = date[0] + "-" + date[1] + "-" + date[2];
    std::string firstOfMonth = date[0] + "-" + date[1] + "-" + "01";
    if (begin.empty() && end.empty())
        query = "?begin_at="+firstOfMonth+"&end_at="+now;
    else if (begin.empty() && !end.empty())
        query = "?begin_at="+firstOfMonth+"&end_at="+end;
    else if (!begin.empty() && !end.empty())
        query = "?begin_at="+begin+"&end_at="+end;
    else if (!begin.empty() && end.empty())
        query = "?begin_at="+begin+"&end_at="+now;
    std::cout << "Query: " << query << std::endl;
    std::string param = "curl -Ss -H \"Authorization: Bearer " + token + "\" " + "\"https://api.intra.42.fr/v2/users/" + login + "/locations_stats" + query + "\"" + " > ./" + login + ".json";
    std::system(param.c_str());
    std::string pathfile = "./"+login+".json";
    std::fstream in(pathfile);
    if (in.is_open()) {
        std::string line;
        std::getline(in, line, '\0');
        line.erase(line.begin());
        line.erase(line.end() - 1);
        if (!line.empty()) {
            std::size_t pos = line.find(",");
            std::string data;
            size_t sIndex;
            size_t eIndex;
            double t = 0;
            while (pos != std::string::npos) {
                data = line.substr(0, pos);
                sIndex = data.find_first_of(":") + 2;
                eIndex = data.find_last_of("\"") - sIndex;
                if (sIndex != std::string::npos && eIndex != std::string::npos) {
                    data = data.substr(sIndex, eIndex);
                    t += std::atoi(data.substr(0, 2).c_str()) * 60 + std::atoi(data.substr(3, 2).c_str());
                }
                line.erase(0, pos + 1);
                pos = line.find_first_of(",");
            }
            sIndex = data.find_first_of(":") + 2;
            eIndex = data.find_last_of("\"") - sIndex;
            if (sIndex != std::string::npos && eIndex != std::string::npos) {
                data = line.substr(sIndex, eIndex);
                t += std::atoi(data.substr(0, 2).c_str()) * 60 + std::atoi(data.substr(3, 2).c_str());
            }
            std::cout << "time: " << t / 60 << std::endl;
            ss << static_cast<int>(t / 60);
        } else {
            param = "rm ./" + login + ".json";
            std::system(param.c_str());
            return "0";
        }
        in.close();
    } else {
        std::cout << "Something went wrong\n";
    }
    param = "rm ./" + login + ".json";
    std::system(param.c_str());
    return ss.str();
}

Bot::~Bot() {
    if (!_instance) {
        delete _instance;
        _instance = nullptr;
    }
}

// curl -X POST --data "grant_type=client_credentials&client_id=u-s4t2ud-0ffa9ea2d9a57c43add851af627d18c9bad19d543662eb62b6291859abe9e921&client_secret=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" https://api.intra.42.fr/oauth/token

// curl  -H "Authorization: Bearer e9fc1f6b87d59a441e92ece5b85e1af862253c40f07a7d2f5c6f4f5d5511b7b8" "https://api.intra.42.fr/v2/users/aaqlzim/locations_stats?begin_at=2023-06-01&end_at=2023-06-10"
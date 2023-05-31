#include "../includes/Request.hpp"

Request::Request(){

}

Request::~Request(){

}

std::string Request::getcmd() const{
    return(this->_cmd);
}

void	Request::setcmd(std::string cmd){
	this->_cmd = cmd;
}

std::vector<std::string> Request::_split(std::string& line, std::string& delim){
	std::vector<std::string> parametrs;
	size_t	pos;
	while (1){
		pos = line.find(delim);
		parametrs.push_back(line.substr(0, pos));	
		line.erase(0, pos + delim.length());
		if (pos == std::string::npos)
			break;
	}
	return(parametrs);
}

std::vector<std::pair<std::string, std::string> > Request::_splitJOIN(std::string& line, std::string& delim){
	std::cout << "Line: " << line << std::endl;
	int first = 1;
	std::string del = " ";
	// line[line.length() - 1] = '\0';
	std::vector<std::pair<std::string, std::string> > parametrs;
	std::vector<std::string> lines = this->_split(line, del);
	size_t	pos_pwd;
	size_t	pos_chann;
	std::string pwd;

	if (!lines.size() || lines.size() > 2)
		return parametrs;
	while (1)
	{
		std::cout << "Lines: " << lines[0] << std::endl;
		pos_chann = lines[0].find(delim);
		if (lines.size() == 2 && first){
			pos_pwd = lines[1].find(delim);
			pwd = lines[1].substr(0, pos_pwd);
			lines[1].erase(0, pos_pwd + delim.length());
			if (pos_pwd == std::string::npos)
				first = 0;
		}
		else
			pwd = "";
		
		parametrs.push_back(std::make_pair(lines[0].substr(0, pos_chann), pwd));	
		lines[0].erase(0, pos_chann + delim.length());
		if (pos_chann == std::string::npos)
			break;
	}
	return(parametrs);
}

void	Request::join_strings(std::vector<std::string>& line){
	std::string	temp;
	bool	flag = false;
	std::vector<std::string>::iterator saver = line.end();
	for (std::vector<std::string>::iterator it = line.begin(); it != line.end(); ++it){
		if ((*it)[0] == ':'){
			flag = true;
			temp = *it;
			saver = it;
		}
		else if (flag)
			temp += " " + *it;
	}
	if (saver != line.end()) {
		line.erase(saver, line.end());
	}
	line.push_back(temp);
}

void Request::parseRequest(std::string& line) {
	std::string	delim = " ";
    size_t  pos = line.find(delim);

    if (pos != std::string::npos){
        this->_cmd = line.substr(0, pos);
		std::transform(this->_cmd.begin(), this->_cmd.end(), this->_cmd.begin(), ::toupper);
		line.erase(0, pos + delim.length());
		if (this->_cmd == "PASS" || this->_cmd == "NICK")
			request.push_back(line.substr(0, line.length()));
		else if (this->_cmd == "USER" || this->_cmd == "PRIVMSG" || this->_cmd == "TOPIC"){
			this->request = _split(line, delim);
			join_strings(this->request);
		}
		else if (this->_cmd == "INVITE" || this->_cmd == "MODE")
			this->request = _split(line, delim);
		else if (this->_cmd == "JOIN" || this->_cmd == "KICK"){
			std::string delim = ",";
			this->_request = _splitJOIN(line, delim);
		}
    }
}

void	Request::setChannel(std::vector<std::string> _vector){
	this->channels = _vector;
}

void	Request::setKeys(std::vector<std::string> _vector){
	this->keys = _vector;
}

void	Request::setRequest(std::vector<std::string> _vector){
	this->request = _vector;
}

void	Request::setRequest_(std::vector<std::pair<std::string, std::string> > _vector){
	this->_request = _vector;
}

std::vector<std::string>	Request::getChannel(void) const{
	return(this->channels);
}

std::vector<std::string>	Request::getRequest(void) const{
	return(this->request);
}

std::vector<std::string>	Request::getkeys(void) const{
	return(this->keys);
}

std::vector<std::pair<std::string, std::string> > Request::getRequest_(void) const{
	return(this->_request);
}

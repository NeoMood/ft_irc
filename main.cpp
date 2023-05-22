#include <iostream>
#include <vector>
#include <string>


std::vector<std::string>	_split(std::string s, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> _v;

	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		_v.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	// if (s.find(";") != std::string::npos)
	// 	s.pop_back();
	_v.push_back(s);
	return _v;
}

int main() {
    std::string line = "#channel1, #channel2 key1, key2";
    std::vector<std::string> v = _split(line, " ");
    std::vector<std::string> tokens;
    for (size_t i = 0; i < v.size(); i++) {
        std::cout << "|" << v.at(i) << "|" << std::endl;
        std::size_t pos = v.at(i).find(",");
        bool invalid = false;
        // if (pos != std::string::npos) {
        //     for (size_t j = 0; j < v.at(i).length(); j++) {
        //         //
        //     }
        // }
    }
    // std::vector<std::string> tokens;
    // for (size_t i = 0; i < tokens.size(); i++) {
    //     std::cout << tokens.at(i) << std::endl;
    // }
    // line.find_first_of(" ");
}
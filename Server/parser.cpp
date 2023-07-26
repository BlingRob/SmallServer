#include "parser.h"
#include "http_answers.h"

#include <iostream>
#include <fstream>

#include <boost/regex.hpp>

std::string Parser::Get(const std::string& request, const std::string& dir)
{
    boost::regex rgx("^(GET) ([a-zA-Z\\/\\.]+)", boost::regex_constants::ECMAScript);
    boost::smatch matchs = *boost::sregex_iterator(request.begin(), request.end(), rgx);

    std::string absPath = dir + matchs[2].str();
    std::cerr << absPath << std::endl;
    std::ifstream file(absPath.c_str());
    
    if (!file.is_open())
        return std::string(HTTP_ANSWERS::NOTFOUND);
    else 
    {
        std::string content;
        getline ( file, content, '\0' );
        std::string answer = HTTP_ANSWERS::OK;
        answer += "Content-length: " + std::to_string(content.size()) + "\r\n";
        answer += "Content-Type: text/html\r\n\r\n";
        answer += content;
        return answer;
    }
}
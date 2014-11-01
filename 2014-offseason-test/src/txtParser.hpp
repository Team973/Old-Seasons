#ifndef TXT_PARSER_HPP
#define TXT_PARSER_HPP

class TxtParser
{
public:
    TxtParser(std::string name);
    std::vector<std::string> getContent();
    std::vector<std::string> getFile();
    std::string scrape(std::string line, char token);
    std::vector<std::string> split(std::string line, char token, int lineNumber);
private:
    FILE *pFile;
    std::vector<std::string> lines;
};

#endif

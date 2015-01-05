#ifndef TXT_PARSER_HPP
#define TXT_PARSER_HPP

namespace frc973 {

class TxtParser
{
public:
    TxtParser(std::string name);
    std::vector<std::string> getContent();

private:
    std::vector<std::string> getFile();

    FILE *pFile;

    std::vector<std::string> lines;
};

}

#endif

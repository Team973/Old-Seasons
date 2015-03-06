#ifndef TXT_FILE_IO_HPP
#define TXT_FILE_IO_HPP

#include "msgChannel.hpp"

namespace frc973 {

class TxtReader
{
public:
    TxtReader(std::string name);
    std::vector<std::string> getContent();
    static void Run();

private:
    std::vector<std::string> getFile();

    FILE *pFile;

    std::vector<std::string> lines;

};

class TxtWriter
{
public:
    static void Write(std::string path, std::string msg);
    static void Run();
private:
    static Channel<std::string> pathChan;
    static Channel<std::string> msgChan;
};

}

#endif

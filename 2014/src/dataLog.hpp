#ifndef DATA_LOG_H
#define DATA_LOG_H

class DataLog
{
public:
    DataLog(std::string filename_);
    void log(std::string data);
    template<typename T>
    std::string asString(T in);
private:
    std::string currTime();
    std::string filename;
};

#endif

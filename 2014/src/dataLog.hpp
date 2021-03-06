#ifndef DATA_LOG_H
#define DATA_LOG_H

class DataLog
{
public:
    DataLog(std::string filename_);
    DataLog();
    void log(std::string data, bool attachTime = true);
    template<typename T>
    std::string asString(T in);
private:
    std::string currSystemTime();
    std::string currDateTime();
    std::string filename;
};

#endif

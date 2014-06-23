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
    void currTime();
    std::string filename;
    std::string timeStamp;
};

#endif

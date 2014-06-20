#ifndef DATA_LOG_H
#define DATA_LOG_H

class DataLog
{
public:
    DataLog(std::string filename_);
    void log(std::string data);
private:
    void currTime();
    std::string filename;
    std::string timeStamp;
};

#endif

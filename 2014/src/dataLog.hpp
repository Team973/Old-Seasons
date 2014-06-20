#ifndef DATA_LOG_H
#define DATA_LOG_H

class DataLog
{
public:
    DataLog(std::string filename_);
    void log(std::string data);
private:
    std::string filename;
};

#endif

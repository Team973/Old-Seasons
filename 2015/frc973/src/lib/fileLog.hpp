#ifndef FILE_LOG_HPP
#define FILE_LOG_HPP

#include <string>

namespace frc973 {

class Channel;

class FileLog {
  public:
      FileLog();
      void Log(std::string msg);
      void Run();
 
  private:
    Channel<std::string> msgChan;
};

}

#endif

#ifndef FILE_LOG_HPP
#define FILE_LOG_HPP

#include <string>
#include "msgChannel.hpp"

namespace frc973 {

class Logger {
  public:
      Logger() : msgChan(20) {
      }
      Logger* getInstance();
      void Log(std::string msg);
      void Run();
 
  private:
    Channel<std::string> msgChan;
    static Logger* instance;
};

}

#endif

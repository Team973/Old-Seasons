#ifndef FILE_LOG_HPP
#define FILE_LOG_HPP

#include <string>
#include "msgChannel.hpp"

namespace frc973 {

class Logger {
  public:
      Logger* getInstance();
      static void Log(std::string msg);
      static void* Run(void*);
 
  private:
    static Channel<std::string> msgChan;
};

}

#endif

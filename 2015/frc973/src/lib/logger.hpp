#ifndef FILE_LOG_HPP
#define FILE_LOG_HPP

#include <string>
#include "msgChannel.hpp"

namespace frc973 {

#define WARNING "[ERROR] "
#define MESSAGE "[MESSAGE] "

class Logger {
  public:
      static void Initialize();
      static void Log(std::string flag, std::string msg);
      static void* Run(void*);
 
  private:
    static Channel<std::string> msgChan;
};

}

#endif

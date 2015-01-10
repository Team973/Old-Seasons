#include "logger.hpp"
#include "txtFileIO.hpp"
#include <unistd.h>

namespace frc973 {

Channel<std::string> Logger::msgChan(3);

// Log is called to log a message.
void Logger::Log(std::string msg) {
    msgChan.send(msg);
}

// Run is run in a separate thread, started when the robot is initialized.
void* Logger::Run(void*) {
    while (true) {
        std::string msg = msgChan.recv();
        TxtWriter::Write("/home/lvuser/log.txt", msg);
        usleep(10000);
    }
}

}

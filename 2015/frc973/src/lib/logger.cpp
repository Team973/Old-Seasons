#include "logger.hpp"

namespace frc973 {

Logger* Logger::instance = NULL;

Logger* Logger::getInstance() {
    if (instance == NULL)
    {
        instance = new Logger();
    }
    return instance;
}

// Log is called to log a message.
void Logger::Log(std::string msg) {
    msgChan.send(msg);
}

// Run is run in a separate thread, started when the robot is initialized.
void Logger::Run() {
    while (true) {
        std::string msg = msgChan.recv();
    }
}

}

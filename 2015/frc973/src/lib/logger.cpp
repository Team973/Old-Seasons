#include "logger.hpp"
#include "txtFileIO.hpp"
#include <time.h>
#include <unistd.h>
#include <string>

namespace frc973 {

Channel<std::string> Logger::msgChan(3);

void Logger::Initialize()
{
    time_t rawTime;
    struct tm * timeInfo;
    char buffer[80];

    time(&rawTime);
    timeInfo = localtime(&rawTime);
    strftime(buffer, 80, "/home/lvuser/log::%x::%X.log", timeInfo);

    rename("/home/lvuser/log.txt", buffer);
}

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

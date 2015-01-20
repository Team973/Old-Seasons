#include "logger.hpp"
#include "txtFileIO.hpp"
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

namespace frc973 {

Channel<std::string> Logger::msgChan(3);

void Logger::Initialize()
{
    time_t rawTime;
    struct tm * timeInfo;
    char buffer[80];

    time(&rawTime);
    timeInfo = localtime(&rawTime);
    strftime(buffer, 80, "/home/lvuser/Logs/log::%F::%X.txt", timeInfo);
    for (unsigned int n=0;n<80;n++)
    {
        if (buffer[n] == '\n')
        {
            strncpy(buffer, buffer, n-1);
        }
    }

    rename("/home/lvuser/log.txt", buffer);
}

// Log is called to log a message.
void Logger::Log(std::string flag, std::string msg) {
    msgChan.send(flag + msg);
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

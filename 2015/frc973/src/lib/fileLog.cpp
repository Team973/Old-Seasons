#include "fileLog.hpp"
#include "msgChannel.hpp"

namespace frc973 {

FileLog::FileLog() {
    msgChan(10);
}

// Log is called to log a message.
void FileLog::Log(std::string msg) {
    msgChan.send(msg);
}

// Run is run in a separate thread, started when the robot is initialized.
void FileLog::Run() {
    while (true) {
        std::string msg = msgChan.recv();
    }
}

}

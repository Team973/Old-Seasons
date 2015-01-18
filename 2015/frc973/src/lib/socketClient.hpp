#ifndef SOCKET_CLIENT_HPP
#define SOCKET_CLIENT_HPP

#include "msgChannel.hpp"
#include <string>

namespace frc973 {

class SocketClient {
public:
    static void Start();
    static void Send(std::string msg);
    static std::string Recv();
    static void Run();
private:
    static int sockfd;

    static Channel<std::string> sendChan;
    static Channel<std::string> recvChan;
};

}
#endif

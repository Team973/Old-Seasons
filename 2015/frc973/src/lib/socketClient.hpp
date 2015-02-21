#ifndef SOCKET_CLIENT_HPP
#define SOCKET_CLIENT_HPP

#include "msgChannel.hpp"
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string>

namespace frc973 {

class SocketClient {
public:
    static void Start();
    static void Run(void*);
    static void UpdateDash(std::string name, double value);
private:
    static void HandleNetInput();
    static void HandleRobotInput();
    static void UpdateRobot(char *key, char *val);

    static int sockfd;
    static fd_set readfds;

    static Channel<std::string> sendChan;
    static Channel<std::string> recvChan;

    static void RecieveInput(int sockfd);
    static int sendall(const int s, const char *buf, int *len);
};

}
#endif

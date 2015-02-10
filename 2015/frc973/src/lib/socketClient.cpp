#include "socketClient.hpp"
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "logger.hpp"


namespace frc973 {

    int SocketClient::sockfd = 0;
    Channel<std::string> SocketClient::sendChan(5);
    Channel<std::string> SocketClient::recvChan(1);

    void SocketClient::Start() {
        struct addrinfo hints, *res;
        struct addrinfo *firstConn;

        bzero(&hints, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo(NULL, "8016", &hints, &res);

        for (firstConn = res; firstConn != NULL; firstConn = firstConn->ai_next) {
            if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
                printf("failed to create socket\n");
                Logger::Log(WARNING, "client: failed to create socket\n");
                continue;
            }

            if ((connect(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
                printf("client: no connection\n");
                Logger::Log(WARNING, "client: no connection\n");
                continue;
            }

            break;
        }

        if (firstConn == NULL) {
            printf("client: failed to connect\n");
            Logger::Log(WARNING, "client: failed to connect\n");
        }
    }
    
    void SocketClient::Send(std::string msg) {
        sendChan.send(msg);
    }

    std::string SocketClient::Recv() {
        std::string msg = recvChan.recvNonBlock();
        if (msg.size() <= 0) {
            return NULL;
        }

        return msg;
    }

    void SocketClient::Run() {
        char buffer[75];

        while (true) {
            while (read(sockfd, buffer, 74) > 0) {
                recvChan.send(buffer);
            }

            std::string msg = sendChan.recv();
            write(sockfd, msg.c_str(), msg.size());
        }
    }
}

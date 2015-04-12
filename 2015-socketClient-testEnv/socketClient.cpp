#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include "msgChannel.hpp"
#include "socketClient.hpp"
#include <map>

#define SERVER_PORT "8081"
#define MAX_MSG_LEN 128

namespace frc973 {
    int SocketClient::sockfd;
    fd_set SocketClient::readfds;

    Channel<std::string> SocketClient::sendChan(8);
    Channel<std::string> SocketClient::recvChan(1);

	std::map<std::string, SocketClient::SocketListener*> SocketClient::listeners;

    void SocketClient::Start() {
        struct addrinfo hints, *res;
        struct addrinfo *firstConn;

        bzero(&hints, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        //fills hints with a linked list of possible connection methods
        getaddrinfo(NULL, SERVER_PORT, &hints, &res);

        //runs through linked list of possible connection methods
        for (firstConn = res; firstConn != NULL; firstConn = firstConn->ai_next) {
            if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
                printf("failed to create socket\n");
                //Logger::Log(WARNING, "client: failed to create socket\n");
                continue;
            }

            if ((connect(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
                printf("client: no connection\n");
                //Logger::Log(WARNING, "client: no connection\n");
                continue;
            }

            break;
        }

        if (firstConn == NULL) {
            printf("client: failed to connect\n");
            //Logger::Log(WARNING, "client: failed to connect\n");
        }

        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
    }
    
    /**
     * Changes the values of constants on the robot.  Should be called only by 
     * RecieveInput.
     */
    void SocketClient::UpdateRobot(char *key, char *val) {
        std::string msg (key);
        if (listeners.find(msg) != listeners.end())
            listeners[msg]->OnValueChange(msg, val);
        else
            printf("No listener found for key '%s' so set event falling through\n", key);
    }

    /**
     * Recieves input from a client socket.  
     * First byte notates how many bits of input there will be.  RecieveInput
     * ensures that it recieves all bits before proceeding.
     */
    void SocketClient::RecieveInput(int sockfd) {
        printf("Recieved user input\n");
        char buffer[MAX_MSG_LEN],
            bytesTotal;
        int bytesRecieved = 0;

        int n = recv(sockfd, &bytesTotal, 1, 0);
        printf("recieving %d bytes\n", bytesTotal);

		if (bytesTotal == 0)
			return;
        
        while (bytesRecieved < bytesTotal) {
            n = recv(sockfd, 
                    buffer + bytesRecieved, 
                    sizeof(buffer) - bytesRecieved, 
                    0);
            printf("chunk: %s\n", buffer + bytesRecieved);
            if (n > 0)
                bytesRecieved += n;
            else
                return;
        }
        printf("msg recieved %s.  bytes recieved %d\n", buffer, bytesRecieved);
        buffer[bytesRecieved] = '\0';

        char *cmd = strtok(buffer, ", ");
        printf("cmd found %s\n", cmd);
        if (strcmp(cmd, "set") == 0) {
            char *key = strtok(NULL, " ,");
            char *val = strtok(NULL, ", ;");
            UpdateRobot(key, val);
        }
    }

    /**
     * Function called to notify socketClient that a constant has been modified.
     * Does not update the dash immediately but puts it on the queue that the
     * dash needs to be updated.  Called in constants.cpp and constants.hpp
     */
    void SocketClient::UpdateDash(std::string name, double value) {
        printf("constant changed by robot!\n");
        std::ostringstream cmdConstruct;
        cmdConstruct << name << ',' << value << ';';
        sendChan.send(cmdConstruct.str());
    }


    void SocketClient::AddListener(std::string var, SocketClient::SocketListener *listener) {
        listeners[var] = listener;
    }

    /**
     * Checks the connection with the server to see if server has said anthing.
     * If the server has said something, processes input by sending it to 
     * RecieveInput
     */
    void SocketClient::HandleNetInput() {
        fd_set readySockets = readfds;

        //Select will change readySockets such that it's full of only sockets
        //with input in them
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 50000;
        if (select(sockfd + 1, &readySockets, NULL, NULL, &tv) == -1) {
            perror("select");
        }

        if (FD_ISSET(sockfd, &readySockets)) {
            RecieveInput(sockfd);
        }
    }

    /**
     * (non-blocking) checks for updates from robot through msgChannel.  Takes
     * updates and sends packets to the outfacing socket.
     */ 
    void SocketClient::HandleRobotInput() {
        if (sendChan.size() > 0) {
            std::string msg = sendChan.recv();
            int len = msg.length();
            sendall(sockfd, msg.c_str(), &len);
        }
    }

    /**
     * Main loop between checking for data recieved over the network and
     * checking for data recieved from the message channel.
     */
    void SocketClient::Run(void* in) {
        while (true) {
            HandleNetInput();
            HandleRobotInput();
        }
    }

    /**
     * Sends data in buffer |buf| of length |len| through socket |s|.
     */
    int SocketClient::sendall(const int s, const char *buf, int *len) {
        int total = 0;
        int bytesLeft = *len;
        int n;

        while (total < *len) {
            n = send(s, buf + total, bytesLeft, 0);
            if (n == -1) { break; }
            total += n;
            bytesLeft -= n;
        }

        *len = total;

        return n == -1 ? -1 : 0;
    }
}

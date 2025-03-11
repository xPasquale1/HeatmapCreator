#pragma once

#include <winsock2.h>
#include "../OpenGL-Library/util.h"

//WICHTIG, bevor man irgendetwas hier erstellen/nutzen kann, MUSS WSAStartup() einmal aufgerufen werden!
//Und am Ende natürlich dann auch wieder WSACleanup()

struct UDPServer{
    SOCKET socket;
    sockaddr_in receiver;
};

struct TCPServer{
    SOCKET socket;
    sockaddr_in receiver;
};

enum MESSAGECODES{
    SEND_POSITION_X,
    SEND_POSITION_Y,
    SEND_SIGNALSTRENGTH,
    ADD_ROUTER,
    RESET_ROUTERS,
    SETSENDIP,
    REQ,
    ACK,
    REQUEST_AVG,
    REQUEST_SCANS,
    SCAN_INFO,
    SEND_STATUS
};

/// @brief Erstellt einen UDP Server auf dem Port port mit einem Timeout von timeoutMillis in Millisekunden für recv-Aufrufe und speichert alle Daten im server struct
/// @param server Das UDP Server struct
/// @param port Der Serverport
/// @param timeoutMillis Das Timeout
/// @return ErrCode
ErrCode createUDPServer(UDPServer& server, u_short port, DWORD timeoutMillis = 10){
    server.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(server.socket == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht erstellen");

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

    server.receiver.sin_family = AF_INET;

    int broadcastEnable = 1;
    if(setsockopt(server.socket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnable, sizeof(broadcastEnable)) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socketoptionen nicht setzen");
    if(setsockopt(server.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutMillis, sizeof(DWORD)) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socketoptionen nicht setzen");

    if(bind(server.socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht binden");
    return SUCCESS;
}

/// @brief Löscht den UDP-Server und alle allokierten Ressourcen
/// @param server Das UDP Server struct
/// @return ErrCode
ErrCode destroyUDPServer(UDPServer& server){
    if(closesocket(server.socket) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Socket schließen");
    return SUCCESS;
}

/// @brief Erstellt einen TCP Server auf dem Port port mit einem Timeout von timeoutMillis in Millisekunden für recv-Aufrufe und speichert alle Daten im server struct
/// @param server Das TCP Server struct
/// @param port Der Serverport
/// @param timeoutMillis Das Timeout
/// @return ErrCode
ErrCode createTCPServer(TCPServer& server, u_short port, DWORD timeoutMillis = 10){
    server.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server.socket == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht erstellen");

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

    server.receiver.sin_family = AF_INET;

    int broadcastEnable = 1;
    if(setsockopt(server.socket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnable, sizeof(broadcastEnable)) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socketoptionen nicht setzen");
    if(setsockopt(server.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutMillis, sizeof(DWORD)) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socketoptionen nicht setzen");

    // if(bind(server.socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht binden");
    return SUCCESS;
}

/// @brief Löscht den TCP-Server und alle allokierten Ressourcen
/// @param server Das TCP Server struct
/// @return ErrCode
ErrCode destroyTCPServer(TCPServer& server){
    if(closesocket(server.socket) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Socket schließen");
    return SUCCESS;
}

//TODO ErrCode

/// @brief Wartet auf den Empfang von UDP Packeten und blockiert, sendet -1 bei Fehlern zurück, optional kann man eine sockaddr_in angeben, welche dann die Sender Informationen beinhaltet
/// @param server Das UDP Server struct
/// @param buffer Ein Empfangspuffer
/// @param bufferSize Die Größe des Empfangspuffer
/// @param transmitter Optional, beschreibt das transmitter struct mit den Daten des Senders
/// @return 
int receiveUDPServer(UDPServer& server, char* buffer, int bufferSize, sockaddr_in* transmitter = nullptr){
    if(!transmitter){
        sockaddr_in peer;
        transmitter = &peer;
        int sizeOfPeer = sizeof(peer);
    }
    int size = sizeof(sockaddr);
    return recvfrom(server.socket, buffer, bufferSize, 0, (sockaddr*)transmitter, &size);
}

/// @brief Wartet auf den Empfang von UDP Packeten und blockiert, sendet -1 bei Fehlern zurück, optional kann man eine sockaddr_in angeben, welche dann die Sender Informationen beinhaltet
/// @param server Das UDP Server struct
/// @param buffer Ein Empfangspuffer
/// @param bufferSize Die Größe des Empfangspuffer
/// @param transmitter Optional, beschreibt das transmitter struct mit den Daten des Senders
/// @return 
int receiveUDPServer(UDPServer& server, char* buffer, int bufferSize, sockaddr_in* transmitter = nullptr){
    if(!transmitter){
        sockaddr_in peer;
        transmitter = &peer;
        int sizeOfPeer = sizeof(peer);
    }
    int size = sizeof(sockaddr);
    return recvfrom(server.socket, buffer, bufferSize, 0, (sockaddr*)transmitter, &size);
}

void changeUDPServerDestination(UDPServer& server, const char* ip, u_short port){
    server.receiver.sin_addr.s_addr = inet_addr(ip);
    server.receiver.sin_port = htons(port);
}

void changeUDPServerDestination(UDPServer& server, u_long ip, u_short port){
    server.receiver.sin_addr.s_addr = ip;
    server.receiver.sin_port = htons(port);
}

int sendMessagecodeUDPServer(UDPServer& server, MESSAGECODES code, const char* buffer, int bufferSize){
    char sendBuffer[80];    //TODO könnte zu klein/groß sein
    int sendBufferLength = 0;
    sendBuffer[0] = code;
    switch(code){
        case REQUEST_AVG:
        case RESET_ROUTERS:
            sendBufferLength = 1;
            break;
        case SETSENDIP:
        case ADD_ROUTER:
            for(int i=0; i < bufferSize; ++i){
                sendBuffer[i+1] = buffer[i];
            }
            sendBufferLength = bufferSize+1;
            break;
        case REQUEST_SCANS:
            sendBuffer[1] = buffer[0];
            sendBuffer[2] = buffer[1];
            sendBufferLength = 3;
            break;
        default: return -1;
    }
    return sendto(server.socket, sendBuffer, sendBufferLength, 0, (sockaddr*)&server.receiver, sizeof(server.receiver));
}

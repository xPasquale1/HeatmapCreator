#pragma once

#include <winsock2.h>
#include "util.h"

//WICHTIG, bevor man irgendetwas hier erstellen/nutzen kann, MUSS WSAStartup() einmal aufgerufen werden!
//Und am Ende natürlich dann auch wieder WSACleanup()

struct UDPServer{
    SOCKET socket;
};

/// @brief Erstellt einen UDP Server auf dem Port port und speichert alle Daten im server struct
/// @param server Das UDP Server struct
/// @param port Der Serverport
/// @return ErrCode
ErrCode createUDPServer(UDPServer& server, u_short port){
    server.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(server.socket == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht erstellen");

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

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

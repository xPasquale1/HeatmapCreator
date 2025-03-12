#pragma once

#include <winsock2.h>
#include "../OpenGL-Library/util.h"

//WICHTIG, bevor man irgendetwas hier erstellen/nutzen kann, MUSS WSAStartup() einmal aufgerufen werden!
//Und am Ende natürlich dann auch wieder WSACleanup()

struct UDPServer{
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
ErrCode createUDPServer(UDPServer& server, u_short port, DWORD timeoutMillis = 10)noexcept{
    server.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(server.socket == INVALID_SOCKET) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht erstellen");

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

//TODO ErrCode

/// @brief Wartet auf den Empfang von UDP Packeten und blockiert, sendet -1 bei Fehlern zurück, optional kann man eine sockaddr_in angeben, welche dann die Sender Informationen beinhaltet
/// @param server Das UDP Server struct
/// @param buffer Ein Empfangspuffer
/// @param bufferSize Die Größe des Empfangspuffer
/// @param transmitter Optional, beschreibt das transmitter struct mit den Daten des Senders
/// @return 
int receiveUDPServer(UDPServer& server, char* buffer, int bufferSize, sockaddr_in* transmitter = nullptr)noexcept{
    if(!transmitter){
        sockaddr_in peer;
        transmitter = &peer;
        int sizeOfPeer = sizeof(peer);
    }
    int size = sizeof(sockaddr);
    return recvfrom(server.socket, buffer, bufferSize, 0, (sockaddr*)transmitter, &size);
}

void changeUDPServerDestination(UDPServer& server, const char* ip, u_short port)noexcept{
    server.receiver.sin_addr.s_addr = inet_addr(ip);
    server.receiver.sin_port = htons(port);
}

void changeUDPServerDestination(UDPServer& server, u_long ip, u_short port)noexcept{
    server.receiver.sin_addr.s_addr = ip;
    server.receiver.sin_port = htons(port);
}

int sendMessagecodeUDPServer(UDPServer& server, MESSAGECODES code, const char* buffer, int bufferSize)noexcept{
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

struct TCPConnection{
    SOCKET listeningSocket = INVALID_SOCKET;
    SOCKET transferSocket = INVALID_SOCKET;
};

ErrCode createTCPConnection(TCPConnection& conn, u_short port)noexcept{
    conn.listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(conn.listeningSocket == INVALID_SOCKET) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht erstellen");

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(conn.listeningSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    u_long mode = 1;
    ioctlsocket(conn.listeningSocket, FIONBIO, &mode);

    if(bind(conn.listeningSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht binden");
    if(listen(conn.listeningSocket, 1)) return ErrCheck(GENERIC_ERROR, "TCP Connection Listen");
    return SUCCESS;
}

ErrCode disconnectTCPConnection(TCPConnection& conn)noexcept{
    if(conn.transferSocket == INVALID_SOCKET) return SUCCESS;
    if(shutdown(conn.transferSocket, SD_BOTH) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Shutdown Transfer Socket");
    if(closesocket(conn.transferSocket) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Transfer Socket schließen");
    conn.transferSocket = INVALID_SOCKET;
    return SUCCESS;
}

ErrCode destroyTCPConnection(TCPConnection& conn)noexcept{
    if(disconnectTCPConnection(conn) != SUCCESS) return GENERIC_ERROR;
    if(closesocket(conn.listeningSocket) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Listening Socket schließen");
    conn.listeningSocket = INVALID_SOCKET;
    return SUCCESS;
}

bool connectionOnTCPConnection(TCPConnection& conn)noexcept{
    return conn.transferSocket != INVALID_SOCKET;
}

/// @brief Diese Funktion testet, ob sich ein Client verbinden will. Diese Funktion blockiert nicht, daher wird
/// SUCCESS auch dann zurückgegeben, wenn sich kein Client verbinden will. Um zu testen, ob ein Client verbunden
/// ist, sollte connectionOnTCPConnection genutzt werden! Falls bereits ein Client verbunden ist, wird SUCCESS
/// zurückgegeben.
/// @param conn 
/// @return GENERIC_ERROR bei Fehlern, sonst SUCCESS
ErrCode listenTCPConnection(TCPConnection& conn, DWORD timeoutMillis = 100)noexcept{
    if(connectionOnTCPConnection(conn)) return SUCCESS;
    sockaddr receiver;
    int clientSize = sizeof(receiver);
    conn.transferSocket = accept(conn.listeningSocket, &receiver, &clientSize);
    if(conn.transferSocket == INVALID_SOCKET){
        if(WSAGetLastError() == WSAEWOULDBLOCK) return SUCCESS;
        return ErrCheck(GENERIC_ERROR, "TCP Connection Listen neuer Socket");
    }
    if(setsockopt(conn.transferSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutMillis, sizeof(DWORD)) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Konnte Socketoptionen nicht setzen");
    return SUCCESS;
}

ErrCode connectTCPConnection(TCPConnection& conn, const char* ip, u_short port, DWORD timeoutMillis = 100)noexcept{
    if(disconnectTCPConnection(conn) != SUCCESS) return GENERIC_ERROR;
    conn.transferSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(conn.transferSocket == INVALID_SOCKET) return ErrCheck(GENERIC_ERROR, "Konnte Socket nicht erstellen");

    sockaddr_in targetAddr = {};
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(port);
    targetAddr.sin_addr.s_addr = inet_addr(ip);

    u_long mode = 1;
    ioctlsocket(conn.transferSocket, FIONBIO, &mode);

    if(connect(conn.transferSocket, (sockaddr*)&targetAddr, sizeof(targetAddr)) == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK){
        if(closesocket(conn.transferSocket) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Listening Socket schließen");
        conn.transferSocket = INVALID_SOCKET;
        return GENERIC_ERROR;
    }

    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(conn.transferSocket, &writeSet);

    timeval timeout;
    timeout.tv_sec = timeoutMillis / 1000;
    timeout.tv_usec = (timeoutMillis % 1000) * 1000;

    int result = select(0, nullptr, &writeSet, nullptr, &timeout);

    if(result <= 0){
        if(result == 0){
            closesocket(conn.transferSocket);
            conn.transferSocket = INVALID_SOCKET;
            return ErrCheck(GENERIC_ERROR, "Verbindung timeout");
        }else{
            closesocket(conn.transferSocket);
            conn.transferSocket = INVALID_SOCKET;
            return ErrCheck(GENERIC_ERROR, "Fehler beim select()");
        }
    }

    if(FD_ISSET(conn.transferSocket, &writeSet)) return SUCCESS;

    closesocket(conn.transferSocket);
    conn.transferSocket = INVALID_SOCKET;
    return ErrCheck(GENERIC_ERROR, "Verbindung fehlgeschlagen");

    return SUCCESS;
}

/// @brief 
/// @param conn 
/// @param buffer 
/// @param bufferSize 
/// @return > 0 bei Erfolg, 0 falls Verbindung geschlossen/keine existiert, sonst SOCKET_ERROR
int receiveTCPConnection(TCPConnection& conn, char* buffer, int bufferSize)noexcept{
    if(!connectionOnTCPConnection(conn)) return 0;
    int ret = recv(conn.transferSocket, buffer, bufferSize, 0);
    if (ret <= 0) { 
        if(ret == 0 || WSAGetLastError() == WSAECONNRESET){ 
            if(closesocket(conn.transferSocket) == SOCKET_ERROR) return ErrCheck(GENERIC_ERROR, "Transfer Socket schließen");
            conn.transferSocket = INVALID_SOCKET;
        }
    }
    return ret;
}

/// @brief Sendet eine Nachricht an den aktuell verbundenen Client
/// @param conn 
/// @param code 
/// @param buffer 
/// @param bufferSize 
/// @return Die Anzahl der gesendeten Bytes bei Erfolg, 0 falls keine Verbindung vorhanden ist, sonst SOCKET_ERROR
int sendMessagecodeTCPConnection(TCPConnection& conn, MESSAGECODES code, const char* buffer, int bufferSize)noexcept{
    char sendBuffer[80];    //TODO könnte zu klein/groß sein
    int sendBufferLength = 0;
    sendBuffer[0] = code;
    if(!connectionOnTCPConnection(conn)) return 0;
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
    return send(conn.transferSocket, sendBuffer, sendBufferLength, 0);
}

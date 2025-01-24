//
// Created by theo on 23/01/2025.
//

#include <Socket/Socket.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define NOMINMAX

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>

static bool bIsWinSockInitialized = false;
static int socketCount = 0;

static WSADATA wsaData;

bool InitializeWinSock() {
    if (bIsWinSockInitialized) return true;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }

    bIsWinSockInitialized = true;

    return true;
}

void DeinitializeWinSock() {
    if (!bIsWinSockInitialized) return;

    WSACleanup();

    bIsWinSockInitialized = false;
}

void TryDeinitializeWinSock() {
    if (socketCount > 0) return;

    DeinitializeWinSock();
}

struct Socket::SocketData {
    SOCKET socket = INVALID_SOCKET;
};

Socket::Socket() {
    if (!InitializeWinSock()) {
        throw std::exception("Couldn't create socket: Winsock couldn't be initialized.");
    }

    mSocketData = std::make_unique<SocketData>();
    mSocketData->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    socketCount++;
}

Socket::~Socket() {
    closesocket(mSocketData->socket);

    socketCount--;

    TryDeinitializeWinSock();
}

bool Socket::Bind(const std::string &port) {
    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));

    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(std::atoi(port.c_str()));
    address.sin_family = AF_INET;

    if (bind(mSocketData->socket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == SOCKET_ERROR) {
        return false;
    }

    return true;
}

bool Socket::Listen(Message &out, size_t maxSize) const {
    sockaddr_in address;
    int addressSize = sizeof(address);
    std::memset(&address, 0, sizeof(addressSize));

    // Storage buffer
    char *buffer = new char[maxSize];

    int byteRead = recvfrom(mSocketData->socket, buffer, maxSize, 0, reinterpret_cast<sockaddr *>(&address),
                            &addressSize);
    if (byteRead == SOCKET_ERROR) {
        return false;
    }


    // On copie le message reçu dans le buffer de sortie
    out.data.resize(byteRead);
    std::memcpy(out.data.data(), buffer, byteRead);

    char strAddr[INET_ADDRSTRLEN];
    inet_ntop(address.sin_family, &address.sin_addr, strAddr, INET_ADDRSTRLEN);
    out.address = strAddr;
    out.port = std::to_string(ntohs(address.sin_port));

    delete[] buffer;

    return true;
}

bool Socket::Send(const std::string &address, const std::string &port, const std::vector<char> &message) const {
    sockaddr_in address_sa;
    std::memset(&address_sa, 0, sizeof(address_sa));
    address_sa.sin_family = AF_INET;
    address_sa.sin_port = htons(std::atoi(port.c_str()));
    inet_pton(AF_INET, address.c_str(), &address_sa.sin_addr);

    if (sendto(mSocketData->socket, message.data(), message.size(), 0, reinterpret_cast<sockaddr *>(&address_sa),
               sizeof(address_sa)) == SOCKET_ERROR) {
        return false;
    }

    return true;
}

std::string Socket::GetLastErrorMessage() {
    // https://stackoverflow.com/questions/3400922/how-do-i-retrieve-an-error-string-from-wsagetlasterror
    char *message = nullptr;
    int error = WSAGetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                  NULL,
                  error,
                  MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPSTR)&message,
                  0,
                  NULL);


    std::stringstream ss;
    ss << "Error " << error << ": " << message;

    LocalFree(message);

    return ss.str();
}

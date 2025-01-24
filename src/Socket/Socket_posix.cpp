//
// Created by theo on 23/01/2025.
//

#include <Socket/Socket.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <cstring>

Socket::Socket() {
    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

Socket::~Socket() {
    close(mSocket);
}

bool Socket::Bind(const std::string &port) {
    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));

    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(std::atoi(port.c_str()));
    address.sin_family = AF_INET;

    if (bind(mSocket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1) {
        return false;
    }

    return true;
}

bool Socket::Listen(Message &out, size_t maxSize) const {
    sockaddr_in address;
    socklen_t addressSize = sizeof(address);
    std::memset(&address, 0, sizeof(addressSize));

    // Storage buffer
    char *buffer = new char[maxSize];

    int byteRead = recvfrom(mSocket, buffer, maxSize, 0, reinterpret_cast<sockaddr *>(&address), &addressSize);
    if (byteRead == -1) {
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

    if (sendto(mSocket, message.data(), message.size(), 0, reinterpret_cast<sockaddr *>(&address_sa), sizeof(address_sa)) == -1) {
        return false;
    }

    return true;
}

std::string Socket::GetLastErrorMessage() {
    std::stringstream ss;
    ss << "Error " << ": ";

    return ss.str();
}

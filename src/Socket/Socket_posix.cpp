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

Socket::Socket(SocketType type) {
    switch (type) {
        case SocketType::IPV6: {
            mSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
            break;
        }
        case SocketType::IPV4: {
            mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            break;
        }
    }
}

Socket::~Socket() {
    close(mSocket);
}

bool Socket::Bind(const std::string &port) {
    sockaddr_in6 address;
    std::memset(&address, 0, sizeof(address));

    address.sin6_addr = in6addr_any;
    address.sin6_port = htons(std::atoi(port.c_str()));
    address.sin6_family = AF_INET6;

    int v60OnlyEnabled = 0;
    if (setsockopt(mSocket, IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &v60OnlyEnabled, sizeof(v60OnlyEnabled)) != 0)
        return false;

    if (bind(mSocket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1) {
        return false;
    }

    return true;
}

bool Socket::Listen(Message &out, size_t maxSize) const {
    sockaddr_in6 address;
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

    char strAddr[INET6_ADDRSTRLEN];
    inet_ntop(address.sin6_family, &address.sin6_addr, strAddr, INET6_ADDRSTRLEN);
    out.address = strAddr;
    out.port = std::to_string(ntohs(address.sin6_port));

    delete[] buffer;

    return true;
}

bool Socket::Send(const std::string &address, const std::string &port, const std::vector<char> &message) const {
    sockaddr_in6 address_sa;
    std::memset(&address_sa, 0, sizeof(address_sa));
    address_sa.sin6_family = AF_INET6;
    address_sa.sin6_port = htons(std::atoi(port.c_str()));
    inet_pton(AF_INET6, address.c_str(), &address_sa.sin6_addr);

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

//
// Created by theo on 23/01/2025.
//

#pragma once

#include <string>
#include <memory>
#include <vector>

struct Message {
    std::vector<char> data; // TODO Change to span
    std::string address;
    std::string port;
};

enum class SocketType {
    IPV6,
    IPV4
};

// TODO Not use gigantic switch cases
// Sidenote : why did we do this ?
// Also should have either used defines

class Socket {
public:
    // TODO Dont do this ?
    Socket(SocketType type = SocketType::IPV6);

    ~Socket();

    bool Bind(const std::string &port);

    bool Listen(Message &out, size_t maxSize = 1024) const;

    // TODO Not const
    // TODO Rename
    bool Send(const std::string &address, const std::string &port, const std::vector<char> &message) const;

    static std::string GetLastErrorMessage();

private:
    // TODO Change, this is trash
    int mSocket = 0;
    SocketType mSocketType;
};
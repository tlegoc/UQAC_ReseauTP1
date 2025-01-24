//
// Created by theo on 23/01/2025.
//

#pragma once

#include <string>
#include <memory>
#include <vector>

struct Message {
    std::vector<char> data;
    std::string address;
    std::string port;
};

class Socket {
public:
    Socket();

    ~Socket();

    bool Bind(const std::string &port);

    bool Listen(Message &out, size_t maxSize = 1024) const;

    bool Send(const std::string &address, const std::string &port, const std::vector<char> &message) const;

    static std::string GetLastErrorMessage();

private:
    // Avoid making a virtual class by overriding this in cpp files
    int mSocket = 0;
};
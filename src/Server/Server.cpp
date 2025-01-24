//
// Created by theo on 23/01/2025.
//

#include <Socket/Socket.h>
#include <iostream>

int main()
{
    Socket socket;

    if (!socket.Bind("5555"))
    {
        std::cerr << "Couldn't bind socket: " << Socket::GetLastErrorMessage() << std::endl;
        return 1;
    }

    Message message;
    do
    {
        if (!socket.Listen(message)) {
            std::cerr << "Couldn't listen to socket: " << Socket::GetLastErrorMessage() << std::endl;
            return 1;
        }

        std::cout << "Received message from " << message.address << ":" << message.port << std::endl;
        std::cout << "Message: " << std::string(message.data.begin(), message.data.end()) << std::endl;

        socket.Send(message.address, message.port, message.data);

        if (std::strcmp(std::string(message.data.begin(), message.data.end()).c_str(), "exit") == 0)
        {
            break;
        }
    } while (true);

    return 0;
}
//
// Created by theo on 24/01/2025.
//

#include <Socket/Socket.h>

#include <iostream>
#include <cstring>

int main() {
    Socket socket;

    do {
        std::string text;

        std::cout << "Enter a message: ";
        std::getline(std::cin, text);

        std::vector<char> message;

        message = std::vector<char>(text.begin(), text.end());

        socket.Send("127.0.0.1", "5555", message);

        Message returnMessage;

        if (!socket.Listen(returnMessage)) {
            std::cerr << "Couldn't listen to socket: " << Socket::GetLastErrorMessage() << std::endl;
            return 1;
        }

        std::string returnMessageStr(returnMessage.data.begin(), returnMessage.data.end());
        std::cout << "Result from server: " << returnMessageStr << std::endl;

        if (text == "exit") {
            break;
        }
    } while (true);

    return 0;
}
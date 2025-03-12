#include <iostream>
#include <unistd.h>
#include <cstring>

int main()
{
    char buffer[256];

    // Read from stdin (
    while (true)
    {
        ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytesRead <= 0)
            break; // If no data is read, exit

        if (std::strcmp(buffer, "quit") == 0)
        {
            std::cout << "Quitting Program: " << std:: endl;
            break; // Exit the loop if 'quit' is received
        }

        // Otherwise, it’s a number, increment it and output
        int number;
        std::memcpy(&number, buffer, sizeof(number));
        std::cout << "Received: " << number << ", Sending back: " << number + 1 << std::endl;
    }

    return 0;
}

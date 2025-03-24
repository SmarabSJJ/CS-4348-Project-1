#include <iostream>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <string>

using namespace std;

string trim(const string &str)
{
    string trimmed = str;
    trimmed.erase(trimmed.find_last_not_of(" \n\r\t") + 1); // Remove trailing whitespace
    return trimmed;
}





int main()
{
    char buffer[256];

    // Read from stdin (
    while (true)
    {
        ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytesRead <= 0)
            break; // If no data is read, exit

        cerr << "Made it here\n";

        if (std::strcmp(buffer, "quit") == 0)
        {
            std::cout << "Quitting Program: " << std::endl;
            break; // Exit the loop if 'quit' is received
        }

        // Otherwise, it’s a number, increment it and output
        int number;
        number = std::stoi(buffer);
        std::cout << "Received: " << to_string(number) << ", Sending back: " << to_string(number + 1) << std::endl;
    }

    return 0;
}

/*int main()
{
    string input;

    // Read input from stdin line-by-line
    while (getline(cin, input)) // Reads until newline '\n'
    {
        cerr << "Made it here " << "Input is : '" << input << "'" << endl;

        // input = trim(input);

        if (input == "quit")
        {
            cerr << "Quitting Program: cerr LOL " << endl;
            cout << "Quitting Program: " << endl;
            break; // Exit the loop if 'quit' is received
        }

        // Convert input to number

        try
        {
            int num;
            istringstream iss(input);
            iss >> num;              // Convert string to int
            cout << num + 1 << endl; // Send back incremented number
        }
        catch (invalid_argument &)
        {
            cerr << "Invalid input received in child process." << endl;
        }

        input.clear();
    }

    return 0;
}
*/

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>

using namespace std;

int main()
{
    int pipefd[2];
    pid_t pid;

    // Create the pipe
    if (pipe(pipefd) == -1)
    {
        cerr << "Pipe failed." << endl;
        return 1;
    }

    // Fork the process
    pid = fork();

    if (pid < 0)
    {
        cerr << "Fork failed." << endl;
        return 1;
    }

    if (pid > 0)
    {
        // Parent process
        close(pipefd[0]); // Close unused read end of the pipe

        string input;
        while (true)
        {
            cout << "Enter a number or 'quit' to exit: ";
            getline(cin, input);

            // If user types quit, send "quit" to the child and exit
            if (input == "quit")
            {
                write(pipefd[1], input.c_str(), input.length() + 1);
                break;
            }

            // Validate the input is a number
            istringstream iss(input);
            int number;
            if (iss >> number)
            {
                // Send valid number to child process
                write(pipefd[1], &number, sizeof(number));
            }
            else
            {
                cout << "Invalid input, please enter a number or 'quit'." << endl;
            }
        }

        close(pipefd[1]); // Close write end after sending "quit"
        wait(NULL);       // Wait for child process to finish
    }
    else
    {
        // Child process
        close(pipefd[1]); // Close unused write end of the pipe

        dup2(pipefd[0], STDIN_FILENO); // Use the pipe's read end as standard input

        // Prepare arguments to execvp
        char *args[] = {"./child", NULL}; // Assuming child program is named "child"

        execvp(args[0], args); // Execute the child program
        cerr << "Execvp failed." << endl;
        return 1;
    }

    return 0;
}

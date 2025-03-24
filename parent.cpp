#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>
#include <fcntl.h>

using namespace std;

int main()
{
    int pipePtoCfd[2], pipeCtoPfd[2];
    pid_t pid;

    // Create the pipe
    if (pipe(pipePtoCfd) || pipe(pipeCtoPfd) == -1)
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
        close(pipePtoCfd[0]); // Close unused read end of the pipe
        close(pipeCtoPfd[1]);

        // fcntl(pipeCtoPfd[0], F_SETFL, O_NONBLOCK);

        string input;
        while (true)
        {
            cout << "Enter a number or 'quit' to exit: ";
            getline(cin, input);

            // If user types quit, send "quit" to the child and exit
            if (input == "quit")
            {
                input = input + "\n";
                write(pipePtoCfd[1], input.c_str(), input.length() + 1);

                close(pipePtoCfd[1]);

                int status;
                wait(&status);
                break;
            }

            // Validate the input is a number
            istringstream iss(input);
            int number;
            if (iss >> number)
            {
                // Send valid number to child process
                input = input + "\n";
                write(pipePtoCfd[1], input.c_str(), input.length() + 1);
            }
            else
            {
                cout << "Invalid input, please enter a number or 'quit'." << endl;
            }

            char buffer[1024];
            ssize_t bytesRead;

            bytesRead = read(pipeCtoPfd[0], buffer, sizeof(buffer) - 1);

            buffer[bytesRead] = '\0'; // Null-terminate the received data

            cout << "Gayy:: " << buffer << std::endl;
        }

        close(pipePtoCfd[1]); // Close write end after sending "quit"
        wait(NULL);           // Wait for child process to finish
    }
    else
    {
        // Child process
        close(pipePtoCfd[1]); // Close unused write end of the pipe
        close(pipeCtoPfd[0]);

        dup2(pipePtoCfd[0], STDIN_FILENO); // Use the pipe's read end as standard input
        dup2(pipeCtoPfd[1], STDOUT_FILENO);
        // Prepare arguments to execvp
        char *args[] = {"./child", NULL}; // Assuming child program is named "child"

        execvp(args[0], args); // Execute the child program
        cerr << "Execvp failed." << endl;
        return 1;
    }

    return 0;
}

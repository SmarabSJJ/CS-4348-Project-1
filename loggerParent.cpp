#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>

using namespace std;

void toLower(string &command)
{
    transform(command.begin(), command.end(), command.begin(), [](unsigned char c)
              { return tolower(c); });
}
void toUpper(string &command)
{
    transform(command.begin(), command.end(), command.begin(), [](unsigned char c)
              { return toupper(c); });
}

int main()
{
    int pipePtoCfd[2], pipeCtoPfd[2];
    string command, arguement;
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
            cout << "Enter here: " << endl;
            getline(cin, input);

            // If user types quit, send "quit" to the child and exit
            if (input == "exit")
            {
                write(pipePtoCfd[1], input.c_str(), input.length() + 1);
                break;
            }

            istringstream iss(input);
            iss >> command >> arguement;
            toLower(command);
            toLower(arguement);
            if (command == "passkey" || command == "encrypt" || command == "decrypt" || command == "history")
                write(pipePtoCfd[1], input.c_str(), input.length() + 1);

            char buffer1[1024];
            ssize_t bytesRead;

            bytesRead = read(pipeCtoPfd[0], buffer1, sizeof(buffer1) - 1);

            buffer1[bytesRead] = '\0'; // Null-terminate the received data

            cout << buffer1 << endl;
        }

        close(pipePtoCfd[1]); // Close write end after sending "quit"
        close(pipeCtoPfd[0]);
        wait(NULL); // Wait for child process to finish
    }
    else
    {
        // Child process
        close(pipePtoCfd[1]); // Close unused write end of the pipe
        close(pipeCtoPfd[0]);

        dup2(pipePtoCfd[0], STDIN_FILENO); // Use the pipe's read end as standard input
        dup2(pipeCtoPfd[1], STDOUT_FILENO);
        // Prepare arguments to execvp
        char *args[] = {"./Vingere", NULL}; // Assuming child program is named "child"
        execvp(args[0], args);              // Execute the child program

        cerr << "Execvp failed." << endl;
        return 1;
    }

    return 0;
}

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Please type file namer in command";
        return 1;
    }

    string fileName = argv[1];

    ofstream file(fileName, ios::app);
    if (!file)
    {
        cerr << "Please type correct file name like 'sample.txt'\n";
    }
    file.close();

    int pipeMaintoEncoder[2], pipeEncodertoMain[2], pipeMaintoLogger[2], pipeLoggertoMain[2]; // create pipes for two programs
    pid_t pipEnc, pipLog;                                                                     // process ids

    if (pipe(pipeMaintoEncoder) || pipe(pipeLoggertoMain) || pipe(pipeEncodertoMain) || pipe(pipeMaintoLogger) == -1)
    {
        perror("Pipe creation failed");
        return 1;
    }

    pipEnc = fork();

    if (pipEnc < 0)
    {
        perror("Fork Failed for Encoder");
        return 1;
    }

    if (pipEnc == 0) // in the Encoder process
    {
        // Close unused write end of the pipe
        close(pipeMaintoEncoder[1]);
        close(pipeEncodertoMain[0]);
        dup2(pipeMaintoEncoder[0], STDIN_FILENO);
        char *args[] = {"./Vingere", NULL};

        execvp(args[0], args); // Execute the child program
        cerr << "Execvp failed." << endl;
        return 1;
    }

    /*
    pipLog = fork();

    if (pipLog < 0)
    {
        perror("Fork Failed for Encoder");
        return 1;
    }

    if (pipLog == 0) // in the Logger process
    {
        close(pipeLogger[1]); // Close unused write end of the pipe
        dup2(pipeLogger[0], STDIN_FILENO);
        char *args[] = {"./Logger", const_cast<char*>(fileName.c_str()), NULL};

        execvp(args[0], args); // Execute the child program
        cerr << "Execvp failed." << endl;
        return 1;
    }
    */

    string input;
    while (true)
    {
        close(pipeEncoder[0]);
        cout << "Enter command for Vingere:" << endl;

        getline(cin, input);

        if (input == "quit")
        {
            write(pipeEncoder[1], input.c_str(), input.length() + 1);
            break;
        }

        write(pipeEncoder[1], input.c_str(), input.length() + 1);
    }

    return 0;
}

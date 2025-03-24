#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <cctype>

using namespace std;

void displayMenu()
{
    cout << endl
         << "----------------------------------\n";
    cout << "              Menu               \n";
    cout << "----------------------------------\n";
    cout << "password  - set the password for encryption/decryption\n";
    cout << "encrypt   - encrypt a string\n";
    cout << "decrypt   - decrypt a string\n";
    cout << "history   - show history\n";
    cout << "quit      - quit program\n";
    cout << "----------------------------------\n";
    cout << "\nEnter Command: ";
}

void printHistory(const vector<string> &history)
{
    cout << endl
         << "----------------------------------\n";
    cout << "            History               \n";
    cout << "----------------------------------\n";

    // Print stored strings
    for (size_t i = 0; i < history.size(); ++i)
    {
        cout << i + 1 << ". " << history[i] << "\n";
    }

    // Option to go back
    cout << history.size() + 1 << ". (Go Back)\n";
    cout << "----------------------------------\n";
}

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

    if (pipEnc == 0) // In the Encoder process (Child)
    {
        // Close unused pipe ends
        close(pipeMaintoEncoder[1]); // Child doesn't need to write to this pipe
        close(pipeEncodertoMain[0]); // Child doesn't need to read from this pipe

        // Redirect standard input and output to the pipes
        dup2(pipeMaintoEncoder[0], STDIN_FILENO);
        dup2(pipeEncodertoMain[1], STDOUT_FILENO);

        // Execute the child process
        char *args[] = {"./Vingere", NULL};
        execvp(args[0], args);

        // If execvp fails, print error and exit
        cerr << "Execvp failed." << endl;
        exit(1);
    }

    pipLog = fork();

    if (pipLog < 0)
    {
        perror("Fork Failed for Encoder");
        return 1;
    }

    if (pipLog == 0) // in the Logger process
    {
        close(pipeMaintoLogger[1]); // Child doesn't need to write to this pipe
        close(pipeLoggertoMain[0]); // Child doesn't need to read from this pipe

        // Redirect standard input and output to the pipes
        dup2(pipeMaintoLogger[0], STDIN_FILENO);
        dup2(pipeLoggertoMain[1], STDOUT_FILENO);

        char *args[] = {"./Logger", const_cast<char *>(fileName.c_str()), NULL};

        execvp(args[0], args); // Execute the child program
        cerr << "Execvp failed." << endl;
        return 1;
    }
    vector<string> hist;
    string input, send, toLogger, Result;
    string command, arguement;
    char buffer1[1024];
    ssize_t bytesRead;
    bool goBack = false;
    close(pipeMaintoEncoder[0]); // Parent doesn't need these pipes so close
    close(pipeEncodertoMain[1]);
    close(pipeMaintoLogger[0]);
    close(pipeLoggertoMain[1]);
    while (true) // in parent program only now
    {
        goBack = false;
        displayMenu();

        getline(cin, input);

        if (input == "quit")
        {
            input = "exit";
            write(pipeMaintoEncoder[1], input.c_str(), input.length() + 1);
            write(pipeMaintoLogger[1], input.c_str(), input.length() + 1);
            break;
        }
        istringstream iss(input);
        iss >> command;
        toLower(command);
        string YN, other, Result;
        int num;
        if (command == "password")
        {
            while (1)
            {
                if (hist.size() >= 1)
                {
                    cout << "Would you like to use History?(Y/N) ";

                    getline(cin, YN);

                    if (YN == "Y" || YN == "y")
                    {
                        printHistory(hist);
                        cout << "Select Password: ";
                        getline(cin, other);

                        while (1)
                        {
                            try
                            {
                                num = stoi(other);

                                if (num > 0 && num <= hist.size())
                                {
                                    arguement = hist[num - 1];
                                    toLower(arguement);
                                    cout << "You selected: " << hist[num - 1] << "\n";
                                    break; // Exit loop after valid selection
                                }
                                else if (num == hist.size() + 1)
                                {
                                    cout << "Going back...\n";
                                    goBack = true;
                                    break; // Exit loop when user chooses to go back
                                }
                                else
                                {
                                    cout << "Invalid selection. Try again.\n";
                                    cout << endl
                                         << "Select Password: ";
                                }
                            }
                            catch (exception E)
                            {
                                cout << "Invalid input. Please enter a valid number.\n";
                            }
                            getline(cin, other);
                        }
                        break;
                    }
                    else if (YN == "N" || YN == "n")
                    {

                        cout << "Enter new Password: ";
                        getline(cin, YN);

                        while (1)
                        {
                            if (all_of(YN.begin(), YN.end(), ::isalpha) && YN.length() > 0)
                            {
                                toLower(YN);
                                arguement = YN;
                                // toUpper(YN);
                                // hist.push_back(YN);
                                break;
                            }
                            cout << "\n Incorrect input. Please Enter new Password: ";
                            getline(cin, YN);
                        }
                        break;
                    }
                }
                else
                {
                    cout << "Enter new Password: ";
                    getline(cin, YN);

                    while (1)
                    {
                        if (all_of(YN.begin(), YN.end(), ::isalpha) && YN.length() > 0)
                        {
                            toLower(YN);
                            arguement = YN;
                            // toUpper(YN);
                            // hist.push_back(YN);
                            break;
                        }
                        cout << "\n Incorrect input. Please Enter new Password: ";
                        getline(cin, YN);
                    }
                    break;
                }
            }

            if (goBack == true)
            {
                continue;
            }

            toLogger = "[SET_PASSWORD] Setting passkey.";
            write(pipeMaintoLogger[1], toLogger.c_str(), toLogger.length() + 1);

            send = "passkey " + arguement;

            write(pipeMaintoEncoder[1], send.c_str(), send.length() + 1);

            bytesRead = read(pipeEncodertoMain[0], buffer1, sizeof(buffer1) - 1);

            buffer1[bytesRead] = '\0'; // Null-terminate the received data

            toLogger = "[SET_PASSWORD] Success.";
            write(pipeMaintoLogger[1], toLogger.c_str(), toLogger.length() + 1);
            cout << endl
                 << endl;
        }
        else if (command == "encrypt")
        {
            while (1)
            {
                if (hist.size() >= 1)
                {
                    cout << "Would you like to use History?(Y/N) ";

                    getline(cin, YN);

                    if (YN == "Y" || YN == "y")
                    {
                        printHistory(hist);
                        cout << "Select word to ENCRYPT: ";
                        getline(cin, other);

                        while (1)
                        {
                            try
                            {
                                num = stoi(other);

                                if (num > 0 && num <= hist.size())
                                {
                                    arguement = hist[num - 1];
                                    toLower(arguement);
                                    cout << "You selected: " << hist[num - 1] << "\n";
                                    break; // Exit loop after valid selection
                                }
                                else if (num == hist.size() + 1)
                                {
                                    cout << "Going back...\n";
                                    goBack = true;
                                    break; // Exit loop when user chooses to go back
                                }
                                else
                                {
                                    cout << "Invalid selection. Try again.\n";
                                    cout << endl
                                         << "Select word to ENCRYPT: ";
                                }
                            }
                            catch (exception E)
                            {
                                cout << "Invalid input. Please enter a valid number.\n";
                            }
                            getline(cin, other);
                        }
                        break;
                    }
                    else if (YN == "N" || YN == "n")
                    {

                        cout << "Enter new word to ENCRYPT: ";
                        getline(cin, YN);

                        while (1)
                        {
                            if (all_of(YN.begin(), YN.end(), ::isalpha) && YN.length() > 0)
                            {
                                toLower(YN);
                                arguement = YN;
                                toUpper(YN);
                                hist.push_back(YN);
                                break;
                            }
                            cout << "\n Incorrect input. Please Enter new word to ENCRYPT: ";
                            getline(cin, YN);
                        }
                        break;
                    }
                }
                else
                {
                    cout << "Enter new word to ENCRYPT: ";
                    getline(cin, YN);

                    while (1)
                    {
                        if (all_of(YN.begin(), YN.end(), ::isalpha) && YN.length() > 0)
                        {
                            toLower(YN);
                            arguement = YN;
                            toUpper(YN);
                            hist.push_back(YN);
                            break;
                        }
                        cout << "\n Incorrect input. Please Enter new word to ENCRYPT: ";
                        getline(cin, YN);
                    }
                    break;
                }
            }
            if (goBack == true)
            {
                continue;
            }
            toLogger = "[ENCRYPT] " + arguement + ".";
            write(pipeMaintoLogger[1], toLogger.c_str(), toLogger.length() + 1);

            send = "encrypt " + arguement;

            write(pipeMaintoEncoder[1], send.c_str(), send.length() + 1);

            bytesRead = read(pipeEncodertoMain[0], buffer1, sizeof(buffer1) - 1);

            buffer1[bytesRead] = '\0'; // Null-terminate the received data

            istringstream iss1(buffer1);
            // cout << "FROM ENCODER: " << buffer1 << endl;

            iss1 >> YN >> Result;

            if (YN == "Password")
            {
                toLogger = "[ENCRYPT] Error: Passkey not set..";
            }
            else
            {

                toLogger = "[ENCRYPT] Success: " + Result + ".";

                YN = "Result: " + Result;
                toUpper(Result);
                hist.push_back(Result);
            }

            write(pipeMaintoLogger[1], toLogger.c_str(), toLogger.length() + 1);

            cout << YN << endl;

            cout << "Hit Enter to Continue\n";
            getline(cin, YN);
        }
        else if (command == "decrypt")
        {
            while (1)
            {
                if (hist.size() >= 1)
                {
                    cout << "Would you like to use History?(Y/N) ";

                    getline(cin, YN);

                    if (YN == "Y" || YN == "y")
                    {
                        printHistory(hist);
                        cout << "Select word to DECRYPT: ";
                        getline(cin, other);

                        while (1)
                        {
                            try
                            {
                                num = stoi(other);

                                if (num > 0 && num <= hist.size())
                                {
                                    arguement = hist[num - 1];
                                    toLower(arguement);
                                    cout << "You selected: " << hist[num - 1] << "\n";
                                    break; // Exit loop after valid selection
                                }
                                else if (num == hist.size() + 1)
                                {
                                    cout << "Going back...\n";
                                    goBack = true;
                                    break; // Exit loop when user chooses to go back
                                }
                                else
                                {
                                    cout << "Invalid selection. Try again.\n";
                                    cout << endl
                                         << "Select word to DECRYPT: ";
                                }
                            }
                            catch (exception E)
                            {
                                cout << "Invalid input. Please enter a valid number.\n";
                            }
                            getline(cin, other);
                        }
                        break;
                    }
                    else if (YN == "N" || YN == "n")
                    {

                        cout << "Enter new word to DECRYPT: ";
                        getline(cin, YN);

                        while (1)
                        {
                            if (all_of(YN.begin(), YN.end(), ::isalpha) && YN.length() > 0)
                            {
                                toLower(YN);
                                arguement = YN;
                                toUpper(YN);
                                hist.push_back(YN);
                                break;
                            }
                            cout << "\n Incorrect input. Please Enter new word to DECRYPT: ";
                            getline(cin, YN);
                        }
                        break;
                    }
                }
                else
                {
                    cout << "Enter new word to DECRYPT: ";
                    getline(cin, YN);

                    while (1)
                    {
                        if (all_of(YN.begin(), YN.end(), ::isalpha) && YN.length() > 0)
                        {
                            toLower(YN);
                            arguement = YN;
                            toUpper(YN);
                            hist.push_back(YN);
                            break;
                        }
                        cout << "\n Incorrect input. Please Enter new word to DECRYPT: ";
                        getline(cin, YN);
                    }
                    break;
                }
            }
            if (goBack == true)
            {
                continue;
            }
            toLogger = "[DECRYPT] " + arguement + ".";
            write(pipeMaintoLogger[1], toLogger.c_str(), toLogger.length() + 1);

            send = "decrypt " + arguement;

            write(pipeMaintoEncoder[1], send.c_str(), send.length() + 1);

            bytesRead = read(pipeEncodertoMain[0], buffer1, sizeof(buffer1) - 1);

            buffer1[bytesRead] = '\0'; // Null-terminate the received data

            istringstream iss1(buffer1);
            iss1 >> YN >> Result;
            // cout << "FROM ENCODER: " << buffer1 << endl;

            if (YN == "Password")
            {
                toLogger = "[DECRYPT] Error: Passkey not set..";
            }
            else
            {

                toLogger = "[DECRYPT] Success: " + Result + ".";

                YN = "Result: " + Result;
                toUpper(Result);
                hist.push_back(Result);
            }

            write(pipeMaintoLogger[1], toLogger.c_str(), toLogger.length() + 1);

            cout << YN << endl;

            cout << "Hit Enter to Continue\n";
            getline(cin, YN);
        }
        else if (command == "history")
        {

            cout << endl
                 << "----------------------------------\n";
            cout << "            History               \n";
            cout << "----------------------------------\n";

            // Print stored strings
            for (size_t i = 0; i < hist.size(); ++i)
            {
                cout << i + 1 << ". " << hist[i] << "\n";
            }
            // Option to go back
            cout << "----------------------------------\n";
            cout << "Hit Enter to Continue\n";
            getline(cin, YN);
        }

        /*
        if (command == "password" || command == "encrypt" || command == "decrypt" || command == "history")
            write(pipeEncodertoMain[1], input.c_str(), input.length() + 1);

        char buffer1[1024];
        ssize_t bytesRead;

        bytesRead = read(pipeEncodertoMain[0], buffer1, sizeof(buffer1) - 1);

        buffer1[bytesRead] = '\0'; // Null-terminate the received data

        cout << buffer1 << endl;
        */
    }

    close(pipeMaintoEncoder[1]); // Parent doesn't need these pipes so close
    close(pipeEncodertoMain[0]);
    close(pipeMaintoLogger[1]);
    close(pipeLoggertoMain[0]);

    return 0;
}

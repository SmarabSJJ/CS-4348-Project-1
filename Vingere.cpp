/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <algorithm>
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

/*

for(int i =0; i < 26;i ++){
        for(int j = 0; j < 26; j ++){

            cout << Vingere[i][j] << " ";

        }
        cout << endl;
    }


*/

int main(int argc, char *argv[])
{
    char buffer[512];
    char Vingere[26][26];

    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 26; j++)
        {

            Vingere[i][j] = 'a' + (i + j) % 26;
        }
    }

    bool passSet = false, exitbool = false;
    ;
    string passKey = "";
    string line, command, arguement;

    while (1)
    {
        // cerr << "Child: Made it here:" << endl;
        ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));
        // getline(cin, line);
        if (strcmp(buffer, "exit") == 0)
        {
            cerr << "Quitting Program: " << endl;
            break; // Exit the loop if 'quit' is received
        }
        istringstream iss(buffer);
        iss >> command >> arguement;

        // cerr << "Got input" << endl;
        // cerr << "command: " << command << " arguement: " << arguement;
        toLower(command);
        toLower(arguement);
        // cout << "command: " << command << " arguement: " << arguement;

        if (command == "passkey")
        {

            passKey = arguement;
            passSet = true;
            cout << "RESULT" << endl;
        }
        else if (command == "encrypt")
        {

            if (!passSet)
            {
                cout << "ERROR: Password not set." << endl;
                continue;
            }
            string ret = "";
            int n = arguement.length();
            for (int i = 0; i < n; i++)
            {
                ret = ret + Vingere[arguement[i] - 'a'][passKey[i % passKey.length()] - 'a'];
            }
            toUpper(ret);
            cout << "RESULT " << ret << endl;
        }
        else if (command == "decrypt")
        {

            if (!passSet)
            {
                cout << "ERROR Password not set" << endl;
            }
            string ret = "";
            int n = arguement.length();
            for (int i = 0; i < n; i++)
            {
                int hold = (arguement[i] - passKey[i % passKey.length()]);
                if (hold < 0)
                    hold += 26;
                ret = ret + (char)('a' + hold);
            }
            toUpper(ret);
            cout << "RESULT " << ret << endl;
        }
        else
        {
            continue;
        }

        command = "";
        arguement = "";
    }

    return 0;
}
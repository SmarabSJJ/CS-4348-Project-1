#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <ctime>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Please specify file name only and re-run";
        return 0;
    }

    string fileName = argv[1];

    ofstream file(fileName, ios::app);
    if (!file)
    {
        cerr << "Error Opening File" << endl;
    }

    string curr = "";
    int num = 0;
    char buffer[512];
    while (1)
    {
        ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));

        time_t now = std::time(nullptr);
        tm *localTime = localtime(&now);

        // Format date-time as YYYY-MM-DD HH:MM
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M", localTime);

        if (num == 0)
        {

            file << timeStr << "[START] Logging Started.\n";

            num++;
        }

        if (strcmp(buffer, "exit") == 0)
        {
            file << timeStr << "[STOPPED] Logging Stopped. \n";
            file.close();
            break;
        }
        file << timeStr << " " << buffer << endl;
    }

    return 0;
}
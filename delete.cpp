#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sys/wait.h>

int main()
{
    int pipeParentToChild[2], pipeChildToParent[2];

    // Create two pipes: one for parent -> child, one for child -> parent
    if (pipe(pipeParentToChild) == -1 || pipe(pipeChildToParent) == -1)
    {
        std::cerr << "Pipe creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0)
    {                                // Child process
        close(pipeParentToChild[1]); // Close write end of parent-to-child pipe
        close(pipeChildToParent[0]); // Close read end of child-to-parent pipe

        char buffer[1024];
        ssize_t bytesRead = read(pipeParentToChild[0], buffer, sizeof(buffer));
        if (bytesRead == -1)
        {
            std::cerr << "Error reading from pipe: " << strerror(errno) << std::endl;
            return 1;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the string
        std::string wordToEncrypt = buffer;

        // Encrypt the word (this is a placeholder for encryption logic)
        std::string encryptedWord = "Encrypted_" + wordToEncrypt; // Example encryption

        // Send encrypted word back to the parent
        write(pipeChildToParent[1], encryptedWord.c_str(), encryptedWord.length());
        close(pipeParentToChild[0]);
        close(pipeChildToParent[1]);
        return 0;
    }
    else if (pid > 0)
    {                                // Parent process
        close(pipeParentToChild[0]); // Close read end of parent-to-child pipe
        close(pipeChildToParent[1]); // Close write end of child-to-parent pipe

        // Read word to encrypt from stdin
        std::string word;
        std::cout << "Enter the word to encrypt: ";
        std::cin >> word;

        // Send the word to the child
        write(pipeParentToChild[1], word.c_str(), word.length());
        close(pipeParentToChild[1]); // Close write end after sending

        // Read the encrypted word from the child
        char encryptedBuffer[1024];
        ssize_t bytesRead = read(pipeChildToParent[0], encryptedBuffer, sizeof(encryptedBuffer));
        if (bytesRead == -1)
        {
            std::cerr << "Error reading from pipe: " << strerror(errno) << std::endl;
            return 1;
        }

        encryptedBuffer[bytesRead] = '\0'; // Null-terminate the string
        std::cout << "Encrypted word: " << encryptedBuffer << std::endl;

        close(pipeChildToParent[0]); // Close read end after receiving
        waitpid(pid, NULL, 0);       // Wait for the child process to finish
    }
    else
    {
        std::cerr << "Fork failed: " << strerror(errno) << std::endl;
        return 1;
    }

    return 0;
}

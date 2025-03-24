# My Project One 
By: Smaran Voora


## Description
This project has the ability to run the vingere cipher and log the in put and out puts form the user using 3 different programs. The Logger files take in put and writes it to a specified file. The Vingere file takes a specific amount of inputs, with the anility to crypt and decrypt with a given password. The Driver file combines all three files and allows for proper inputs to be processed as well as the necessary error handling. 

## Instructions for use:

### Prerequisites
Ensure you have **g++** installed. You can check by running:

g++ --version

If not please go downlod the correct version depending on your OS

## After Installation
Run these commands to compile the needed files: 

g++ -o Vingere Vingere.cpp

g++ -o logger logger.cpp

g++ -o driver driver.cpp

Note: Vingere.cpp is the encrypt/decrypt using the vingere program, the logger.cpp is the Logger file, and the driver.cp is the program, which acts as the driver and connects all three programs.

After ensuring all programs are compiled and up to date run this command:

./driver sample.txt

This command will run the program and store the logged data in the sample.txt file


## Extra Notes

Please note the extra files you see a



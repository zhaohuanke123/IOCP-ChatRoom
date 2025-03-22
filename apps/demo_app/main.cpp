#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

int main()
{
    std::string input;
    std::getline(std::cin, input);
    std::cout << input << "\n";
}

#pragma once
#include <iostream>
#include <vector>
#include "VOverlapped.hpp"
#include "WSAContext.hpp"
#include "Server.hpp"

void InitWs2();
void UninitWs32();

int main()
{
    try
    {
        WSAContext ct;
        IOCPServer server("127.0.0.1", 9527);
        server.Start();

        // 保持主线程运行
        while (true)
        {
            Sleep(1);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#pragma once
#include <iostream>
#include "v_overlapped.hpp"
#include "WSAContext.hpp"
#include "server.hpp"

int main()
{
    try
    {
        WSAContext ct;
        iocp_socket::server server("127.0.0.1", 9527);
        server.start();

        // 保持主线程运行
        while (true)
        {
            Sleep(1);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Server error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

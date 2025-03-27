#pragma once
#include <iostream>
#include <net_lib/v_overlapped.hpp>
#include <net_lib/WSAContext.hpp>
#include <net_lib/server.hpp>

int main()
{
    try
    {
        WSAContext ct;
        iocp_socket::server::get_instance()->start();

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

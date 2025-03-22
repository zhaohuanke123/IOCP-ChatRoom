#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <sstream>
#include <vector>

#include "session.hpp"
#include "WSAContext.hpp"
#include "../../build/libs/net_lib/package_handler.h"

#pragma comment(lib, "Ws2_32.lib")

std::shared_ptr<iocp_socket::session> server_session = nullptr;

static DWORD WINAPI recv_thread(LPVOID lpParam)
{
    SOCKET sock = reinterpret_cast<SOCKET>(lpParam);
    char buffer[2048];

    while (true)
    {
        int ret = recv(sock, buffer, sizeof(buffer), 0);
        // std::cout << "code : " <<  WSAGetLastError() << "\n";
        // std::cout << "ret : " <<  ret << "\n";

        if (ret > 0)
        {
            server_session->receive_from_buffer(buffer, ret, [&](const std::string& str)
            {
                std::cout << str << "\n";
            });
        }
        else if (ret == 0)
        {
            std::cout << ("连接已由服务器关闭") << "\n";
            break;
        }
        else
        {
            if (WSAGetLastError() == WSAECONNRESET)
            {
                std::cout << ("连接被强制重置") << "\n";
            }
            else
            {
                std::cout << "接收错误" << "\n";
            }
            break;
        }
    }
    return 0;
}

int main()
{
    WSAContext ct;

    SOCKET server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(9527);

    if (connect(server_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "连接失败: " << WSAGetLastError() << '\n';
        closesocket(server_socket);
        return 1;
    }

    server_session = std::make_shared<iocp_socket::session>(server_socket);

    // 启动接收线程
    const HANDLE rece_thread = CreateThread(nullptr, 0, recv_thread, (LPVOID)(server_socket), 0, nullptr);
    if (!rece_thread)
    {
        std::cerr << "线程创建失败" << '\n';
        return 1;
    }

    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        if (input == "q")
        {
            break;
        }
        std::cout << input << "\n";

        auto data = iocp_socket::package_handler::serialize_data(input);

        if (server_session->send_sync(data) == SOCKET_ERROR)
        {
            std::cerr << "发送失败: " << WSAGetLastError() << '\n';
            break;
        }
    }

    WaitForSingleObject(rece_thread, 2000);
    return 0;
}

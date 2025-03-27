#include <iostream>
#include <string>
#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <vector>

#include <net_lib/session.hpp>
#include <net_lib/WSAContext.hpp>
#include <net_lib/package_handler.hpp>
#include <net_lib/message_type.hpp>

#pragma comment(lib, "Ws2_32.lib")

std::shared_ptr<iocp_socket::session> server_session = nullptr;
iocp_socket::package_dispatcher dispatcher;

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
            iocp_socket::receive_from_buffer(server_session, buffer, ret);
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

// 输出功能列表
// 1. 发送消息给指定用户
// 2. 创建房间
// 3. 加入房间
void print_menu()
{
    // 清理屏幕
    system("cls");
    std::cout << "1. 发送消息给指定用户\n";
    std::cout << "2. 创建房间\n";
    std::cout << "3. 加入房间\n"; 
}

// 打印选中发送的用户功能
void print_send_user()
{
    // 清理屏幕
    system("cls");
    std::cout << "选中序号发送消息给指定用户：\n"; 
}

void register_messageHandler ()
{
    dispatcher.register_message_handler<iocp_socket::get_room_list_message>(
        iocp_socket::message_type::get_room_list, [](const std::shared_ptr<iocp_socket::session> &send_session, const iocp_socket::get_room_list_message &get_room_list)
        {
            std::cout << "get_room_list_message" << std::endl;
        });
}

int main()
{
    // WSA 初始化
    WSAContext ct;


    // 连接服务器socket
    SOCKET server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(9527);

    if (connect(server_socket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "连接失败: " << WSAGetLastError() << '\n';
        closesocket(server_socket);
        return 1;
    }

    // 后续使用session 接受和发送消息
    server_session = std::make_shared<iocp_socket::session>(server_socket);

    // 启动接收线程
    const HANDLE rece_thread = CreateThread(nullptr, 0, recv_thread, (LPVOID)(server_socket), 0, nullptr);
    if (!rece_thread)
    {
        std::cerr << "线程创建失败" << '\n';
        return 1;
    }

    std::string input;
    std::cout << "请输入用户名: ";
    std::cin >> input;
    iocp_socket::login_message login;
    login.username = input;
    auto data = serialize_data(iocp_socket::login, login.to_json());
    if (server_session->send_sync(data) == SOCKET_ERROR)
    {
        std::cerr << "发送失败: " << WSAGetLastError() << '\n';
        return 1;
    }

    while (true)
    {
        print_menu();
        std::cin >> input;
    }

    WaitForSingleObject(rece_thread, 2000);
    return 0;
}

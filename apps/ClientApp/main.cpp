#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <sstream>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

// 线程安全输出锁
class ConsoleLock
{
public:
    ConsoleLock() { InitializeCriticalSection(&cs); }
    ~ConsoleLock() { DeleteCriticalSection(&cs); }

    void print(const char *msg)
    {
        EnterCriticalSection(&cs);
        std::cout << "[服务器] " << msg << std::endl;
        LeaveCriticalSection(&cs);
    }

private:
    CRITICAL_SECTION cs;
};

std::vector<char> SerializeData(int32_t number, const std::string &str)
{
    std::vector<char> buffer;

    // 1. 序列化 int32 (转换为网络字节序)
    int32_t net_number = htonl(number);
    buffer.insert(buffer.end(),
                  reinterpret_cast<char *>(&net_number),
                  reinterpret_cast<char *>(&net_number) + sizeof(int32_t));

    // 2. 序列化 string 长度 (转换为网络字节序)
    // int32_t str_length = static_cast<int32_t>(str.size());
    // int32_t net_str_length = htonl(str_length);
    // buffer.insert(buffer.end(),
    //               reinterpret_cast<char *>(&net_str_length),
    //               reinterpret_cast<char *>(&net_str_length) + sizeof(int32_t));

    // 3. 序列化 string 内容
    buffer.insert(buffer.end(), str.begin(), str.end());

    return buffer;
}

// 接收线程函数
DWORD WINAPI RecvThread(LPVOID lpParam)
{
    SOCKET sock = reinterpret_cast<SOCKET>(lpParam);
    ConsoleLock console;
    char buffer[4096];

    while (true)
    {
        int ret = recv(sock, buffer, sizeof(buffer) - 1, 0); // 保留1字节给终止符

        if (ret > 0)
        {
            buffer[ret] = '\0'; // 添加字符串终止符
            console.print(buffer);
        }
        else if (ret == 0)
        {
            console.print("连接已由服务器关闭");
            break;
        }
        else
        {
            if (WSAGetLastError() == WSAECONNRESET)
            {
                console.print("连接被强制重置");
            }
            else
            {
                console.print("接收错误");
            }
            break;
        }
    }
    return 0;
}

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(9527);

    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "连接失败: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 启动接收线程
    HANDLE hThread = CreateThread(nullptr, 0, RecvThread,
                                  reinterpret_cast<LPVOID>(sock), 0, nullptr);
    if (!hThread)
    {
        std::cerr << "线程创建失败" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::string input;
    while (true)
    {
        // std::getline(std::cin, input);
        std::cin >> input;
        if (input == "q")
        {
            shutdown(sock, SD_SEND);
            break;
        }
        std::cout << input << "\n";
        
        auto data = SerializeData(input.length() + sizeof(int32_t), input);

        int sent = send(sock, data.data(), data.size(), 0);
        if (sent == SOCKET_ERROR)
        {
            std::cerr << "发送失败: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    WaitForSingleObject(hThread, 2000); 
    closesocket(sock);
    return 0;
}

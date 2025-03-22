#include "Server.hpp"
#include <iostream>
#include <thread>
#include <sstream>

IOCPServer::IOCPServer(const char* ip, u_short port)
    : m_listenSocket(INVALID_SOCKET), m_hIocp(NULL), m_running(false)
{
    // 初始化地址结构
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = inet_addr(ip);
    m_serverAddr.sin_port = htons(port);

    InitializeSocket();
    SetupIOCP();
}

IOCPServer::~IOCPServer()
{
    m_running = false;
    if (m_listenSocket != INVALID_SOCKET)
    {
        closesocket(m_listenSocket);
    }
    if (m_hIocp)
    {
        CloseHandle(m_hIocp);
    }
    WSACleanup();
}

void IOCPServer::InitializeSocket()
{
    // 创建监听Socket
    m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket == INVALID_SOCKET)
    {
        throw std::runtime_error("Failed to create socket");
    }

    // 绑定和监听
    if (bind(m_listenSocket, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) == SOCKET_ERROR)
    {
        throw std::runtime_error("Bind failed");
    }

    if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        throw std::runtime_error("Listen failed");
    }
}

void IOCPServer::SetupIOCP()
{
    // 创建IOCP对象
    m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (!m_hIocp)
    {
        throw std::runtime_error("CreateIoCompletionPort failed");
    }

    // 关联监听Socket到IOCP
    if (!CreateIoCompletionPort((HANDLE)m_listenSocket, m_hIocp, 0, 0))
    {
        throw std::runtime_error("Associate socket failed");
    }
}

void IOCPServer::Start()
{
    m_running = true;
    PostAccept(); // 发起第一个Accept请求

    // 创建工作线程
    std::thread worker(&IOCPServer::WorkerThread, this);
    worker.detach();

    std::cout << "Server started on port " << ntohs(m_serverAddr.sin_port) << std::endl;
}

void IOCPServer::PostAccept()
{
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    auto overlapped = std::make_unique<VOverlapped>(clientSocket, IO_ACCEPT);

    char buffer[1024];
    DWORD bytesReceived = 0;

    if (!AcceptEx(m_listenSocket, clientSocket, buffer, 0,
                  sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
                  &bytesReceived, overlapped.get()))
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            throw std::runtime_error("AcceptEx failed");
        }
    }
    overlapped.release();
}

void IOCPServer::PostRecv(SOCKET clientSocket)
{
    auto overlapped = std::make_unique<VOverlapped>(clientSocket, IO_RECV);

    DWORD flags = 0;
    if (WSARecv(clientSocket, &overlapped->m_buf, 1,
                &overlapped->m_dwBytesRecved, &flags, overlapped.get(), NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            throw std::runtime_error("WSARecv failed");
        }
    }
    overlapped.release();
}

void IOCPServer::WorkerThread()
{
    while (m_running)
    {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED overlapped = nullptr;

        BOOL status = GetQueuedCompletionStatus(
            m_hIocp, &bytesTransferred, &completionKey, &overlapped, INFINITE);

        // std::cout << "status: " << status << "\n";
        // DWORD _error = WSAGetLastError();
        // std::cout << "error : " << _error << "\n";

        VOverlapped* vOver = static_cast<VOverlapped*>(overlapped);

        switch (vOver->m_event)
        {
        case IO_ACCEPT:
            // 关联新socket到IOCP
            {
                std::lock_guard<std::mutex> lock(m_connMutex);
                std::cout << "新客户端连接: " << vOver->m_sockClient << std::endl;
                m_activeConnections.emplace(vOver->m_sockClient, std::make_shared<Session>(vOver->m_sockClient));
            }
            CreateIoCompletionPort((HANDLE)vOver->m_sockClient, m_hIocp, 0, 0);
            PostRecv(vOver->m_sockClient);
            PostAccept();
            break;

        case IO_RECV:
            // 处理接收到的数据
            if (bytesTransferred > 0)
            {
                // std::cout << "Received: "
                //           << std::string(vOver->m_buf.buf, bytesTransferred)
                //           << std::endl;
                std::cout << "Received: "
                    << bytesTransferred
                    << std::endl;

                auto fromSession = m_activeConnections[vOver->m_sockClient];

                memcpy(fromSession->ringBuffer + fromSession->curSize, vOver->m_btBuf, bytesTransferred);
                fromSession->curSize += bytesTransferred;
                int offset = 0;
                while (fromSession->curSize - offset >= sizeof(int32_t))
                {
                    // 处理长度
                    int length = ntohl(*reinterpret_cast<const int32_t*>(fromSession->ringBuffer + offset));
                    if (fromSession->curSize - offset < length)
                    {
                        break;
                    }
                    std::cout << "Parsed length (from combined buffer): " << length << std::endl;

                    // 处理一个包
                    std::ostringstream oss;
                    oss << "[客户端" << fromSession->GetID() << "] : " << std::string(
                        fromSession->ringBuffer + offset + 4, length - 4);
                    std::string sendStr = oss.str();
                    for (auto s : m_activeConnections)
                    {
                        auto toSession = s.second;
                        if (toSession == fromSession)
                        {
                            continue;
                        }

                        s.second->SendAsync(sendStr);
                    }
                    // HandlePackage();

                    offset += length;
                };

                if (offset > 0)
                {
                    if (fromSession->curSize - offset > 0)
                    {
                        memcpy(fromSession->ringBuffer, fromSession->ringBuffer + offset,
                               fromSession->curSize - offset);
                    }

                    fromSession->curSize -= offset;
                }
            }
            else
            {
                HandleDisConnect(vOver, bytesTransferred);
                continue;
            }
            PostRecv(vOver->m_sockClient);
            break;
        }
        delete vOver;
    }
}

void IOCPServer::HandleDisConnect(VOverlapped* vOver, DWORD bytesTransferred)
{
    SOCKET clientSocket = vOver ? vOver->m_sockClient : INVALID_SOCKET;
    auto session = m_activeConnections[clientSocket];
    DWORD error = vOver ? WSAGetLastError() : ERROR_INVALID_HANDLE;

    // 识别断开类型（关键判断）
    if (error == WSAECONNRESET || // 连接重置
        error == WSAEDISCON || // 优雅断开
        error == ERROR_NETNAME_DELETED) // 网络名称删除
    {
        std::cout << "客户端[" << session->GetID() << "] 异常断开, 错误码: " << error << std::endl;
        m_activeConnections.erase(clientSocket);
    }
    else if (vOver && bytesTransferred == 0) // 正常关闭
    {
        std::cout << "客户端[" << session->GetID() << "] 主动断开连接" << std::endl;
        m_activeConnections.erase(clientSocket);
    }

    if (vOver)
    {
        delete vOver;
    }
}

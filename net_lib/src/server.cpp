#include <net_lib/server.hpp>
#include <iostream>
#include <thread>
#include <sstream>

namespace iocp_socket
{
    package_dispatcher server::m_dispatcher;

    server::server(const char* ip, u_short port)
        : m_listenSocket(INVALID_SOCKET), m_hIocp(nullptr), m_running(false)
    {
        // 初始化地址结构
        m_serverAddr.sin_family = AF_INET;
        m_serverAddr.sin_addr.s_addr = inet_addr(ip);
        m_serverAddr.sin_port = htons(port);

        initialize_socket();
        setup_iocp();

        m_dispatcher.register_message_handler(message_type::login, [](const std::string& mes)
        {
            std::cout << "Login message received" << std::endl;
            login_message login = login_message::from_json(json::parse(mes));
            std::cout << "Username: " << login.username << std::endl;
            std::cout << "Password: " << login.password << std::endl;
        });
    }

    server::~server()
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

    void server::initialize_socket()
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

    void server::setup_iocp()
    {
        // 创建IOCP对象
        m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
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

    void server::start()
    {
        m_running = true;
        post_accept(); // 发起第一个Accept请求

        // 创建工作线程
        std::thread worker(&server::worker_thread, this);
        worker.detach();

        std::cout << "Server started on port " << ntohs(m_serverAddr.sin_port) << std::endl;
    }

    void server::post_accept()
    {
        SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
        auto overlapped = std::make_unique<v_overlapped>(clientSocket, IO_ACCEPT);

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

    void server::post_recv(SOCKET client_socket)
    {
        auto overlapped = std::make_unique<v_overlapped>(client_socket, IO_RECV);

        DWORD flags = 0;
        if (WSARecv(client_socket, &overlapped->m_buf, 1,
                    &overlapped->m_dwBytesReceived, &flags, overlapped.get(), nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                throw std::runtime_error("WSARecv failed");
            }
        }
        overlapped.release();
    }

    void server::worker_thread()
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

            const auto v_over = static_cast<v_overlapped*>(overlapped);

            switch (v_over->m_event)
            {
            case IO_ACCEPT:
                // 关联新socket到IOCP
                {
                    std::lock_guard<std::mutex> lock(m_connMutex);
                    std::cout << "新客户端连接: " << v_over->m_sockClient << std::endl;
                    m_activeConnections.emplace(v_over->m_sockClient, std::make_shared<session>(v_over->m_sockClient));
                }
                CreateIoCompletionPort((HANDLE)v_over->m_sockClient, m_hIocp, 0, 0);
                post_recv(v_over->m_sockClient);
                post_accept();
                break;

            case IO_RECV:
                // 处理接收到的数据
                if (bytesTransferred > 0)
                {
                    std::cout << "Received: "
                        << bytesTransferred
                        << std::endl;

                    auto fromSession = m_activeConnections[v_over->m_sockClient];

                    fromSession->receive_from_buffer(v_over->m_btBuf, bytesTransferred, [&](const std::string& message)
                    {
                        auto send_message = "客户端[" + std::to_string(fromSession->get_id()) + "] 说: " + message;
                        for (auto kv : m_activeConnections)
                        {
                            auto session = kv.second;
                            if (session == fromSession)
                            {
                                continue;
                            }

                            auto stream = serialize_data(send_message);
                            session->send_async(stream.data(), stream.size());
                        }
                    });
                }
                else
                {
                    handle_dis_connect(v_over, bytesTransferred);
                    continue;
                }
                post_recv(v_over->m_sockClient);
                break;
            default: ;
            }
            delete v_over;
        }
    }

    void server::handle_dis_connect(v_overlapped* v_over, DWORD bytes_transferred)
    {
        SOCKET clientSocket = v_over ? v_over->m_sockClient : INVALID_SOCKET;
        auto session = m_activeConnections[clientSocket];
        DWORD error = v_over ? WSAGetLastError() : ERROR_INVALID_HANDLE;

        // 识别断开类型（关键判断）
        if (error == WSAECONNRESET || // 连接重置
            error == WSAEDISCON || // 优雅断开
            error == ERROR_NETNAME_DELETED) // 网络名称删除
        {
            std::cout << "客户端[" << session->get_id() << "] 异常断开, 错误码: " << error << std::endl;
            m_activeConnections.erase(clientSocket);
        }
        else if (v_over && bytes_transferred == 0) // 正常关闭
        {
            std::cout << "客户端[" << session->get_id() << "] 主动断开连接" << std::endl;
            m_activeConnections.erase(clientSocket);
        }

        if (v_over)
        {
            delete v_over;
        }
    }
}

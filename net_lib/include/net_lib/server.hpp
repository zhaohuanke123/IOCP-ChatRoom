#pragma once
#include <winsock2.h>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "v_overlapped.hpp"
#include "Session.hpp"

namespace iocp_socket
{
    class server
    {
    public:
        server(const char* ip, u_short port);
        ~server();

        void start();

    private:
        void initialize_socket();
        void setup_iocp();
        void post_accept();
        void post_recv(SOCKET client_socket);
        void worker_thread();
        void handle_dis_connect(v_overlapped* v_over, DWORD bytes_transferred);

        std::mutex m_connMutex;
        std::unordered_map<SOCKET, std::shared_ptr<session>,
                           std::hash<SOCKET>, std::equal_to<SOCKET>>
        m_activeConnections;
        SOCKET m_listenSocket;
        HANDLE m_hIocp;
        sockaddr_in m_serverAddr;
        bool m_running;
    };
}

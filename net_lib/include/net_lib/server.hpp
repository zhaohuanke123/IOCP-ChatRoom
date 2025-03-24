#pragma once
#include <winsock2.h>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <net_lib/v_overlapped.hpp>
#include <net_lib/Session.hpp>
#include <net_lib/package_handler.hpp>
#include <net_lib/package_dispatcher.hpp>

namespace iocp_socket
{
    class server
    {
    public:
        server(const char *ip, u_short port);
        ~server();

        void start();
        static void dispatcher(int mt, const std::string &message) {
            m_dispatcher.dispatch_message(mt, message);
        }

    private:
        void initialize_socket();
        void setup_iocp();
        void post_accept();
        void post_recv(SOCKET client_socket);
        void worker_thread();
        void handle_dis_connect(v_overlapped *v_over, DWORD bytes_transferred);

        std::mutex m_connMutex;
        std::unordered_map<SOCKET, std::shared_ptr<session>,
                           std::hash<SOCKET>, std::equal_to<SOCKET>>
            m_activeConnections;
        SOCKET m_listenSocket;
        HANDLE m_hIocp;
        sockaddr_in m_serverAddr;
        bool m_running;

        static package_dispatcher m_dispatcher;

    public:
    };

}

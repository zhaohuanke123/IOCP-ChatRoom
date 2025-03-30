#pragma once
#include <winsock2.h>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <net_lib/v_overlapped.hpp>
#include <net_lib/Session.hpp>
#include <net_lib/package_dispatcher.hpp>
#include <net_lib/user.hpp>
#include <net_lib/message_type.hpp>

namespace iocp_socket
{
    class server
    {
    public:
        server(const char *ip, u_short port);

        ~server();

        void start();

        static void dispatcher(std::shared_ptr<session> session, message_type mt, const std::string &msg)
        {
            m_dispatcher.dispatch_message(std::move(session), mt, msg);
        }

        static server *get_instance()
        {
            static server instance("127.0.0.1", 9527);
            return &instance;
        }

        shared_ptr<session> get_session(SOCKET sock)
        {
            return m_activeConnections[sock];
        }

    private:
        void initialize_socket();

        void setup_iocp();

        void post_accept();

        void post_recv(SOCKET client_socket);

        void worker_thread();

        void handle_dis_connect(v_overlapped *v_over, DWORD bytes_transferred);

        std::mutex m_connMutex;

        /*
        ** 存储临时连接
        */
        std::unordered_map<SOCKET, std::shared_ptr<session> > m_activeConnections;
        std::unordered_map<std::string, std::shared_ptr<user> > m_loginUsers;
        SOCKET m_listenSocket;
        HANDLE m_hIocp;
        sockaddr_in m_serverAddr;
        bool m_running;

        static package_dispatcher m_dispatcher;

    public:
    };
}

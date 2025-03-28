#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <functional>
#include <net_lib/v_overlapped.hpp>

namespace iocp_socket
{
    class session
    {
        static constexpr int buffer_size = 2048;

    public:
        session() : m_sock(INVALID_SOCKET), m_id(-1)
        {
        }

        explicit session(const SOCKET sock) : m_sock(sock), m_id(generate_id())
        {
            std::cout << "Session 创建 : " << m_id << "\n";
        }

        session(const session &) = delete;

        session &operator=(const session &) = delete;

        void session::receive_from_buffer(v_overlapped *overlapped, const DWORD rece_length);

        session(session &&other) noexcept
            : m_sock(std::exchange(other.m_sock, INVALID_SOCKET)),
              m_id(other.m_id) // 移动后保留原ID
        {
        }

        [[nodiscard]] std::string session::get_remote_endpoint() const
        {
            sockaddr_in sa;
            int len = sizeof(sa);
            if (getpeername(m_sock, reinterpret_cast<sockaddr *>(&sa), &len) == SOCKET_ERROR)
            {
                std::cerr << "getpeername failed with error: " << WSAGetLastError() << std::endl;
            }

            printf("Client Ip：%s   ", inet_ntoa(sa.sin_addr));
            printf("Client Port：%d \n\n", ntohs(sa.sin_port));
            return inet_ntoa(sa.sin_addr);
        }

        ~session();

        void send_async(const std::string &message) const;

        void send_async(const char *message) const;

        void send_async(const std::vector<char> message) const;

        void session::send_async(const char stream[], const size_t length) const;

        int send_sync(const std::vector<char> &message) const;

        [[nodiscard]] int get_id() const noexcept { return m_id; }

    private:
        static int generate_id() noexcept;

    private:
        int m_id;
        static std::atomic<int> idGen;

        SOCKET m_sock;

    public:
        char ring_buffer[buffer_size]{};
        int32_t cur_size = 0;

    public:
    };
}

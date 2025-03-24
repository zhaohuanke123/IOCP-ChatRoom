#pragma once
#include "v_overlapped.hpp"
#include <memory>
#include <iostream>
#include <functional>

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

        session(const session&) = delete;
        session& operator=(const session&) = delete;
        void session::receive_from_buffer(const CHAR buffer[], const DWORD rece_length,
                                          const std::function<void(std::string&)>& call_back);

        session(session&& other) noexcept
            : m_sock(std::exchange(other.m_sock, INVALID_SOCKET)),
              m_id(other.m_id) // 移动后保留原ID
        {
        }

        ~session();
        void send_async(const std::string& message) const;
        void session::send_async(const char stream[], const size_t length) const;
        int send_sync(const std::vector<char>& message) const;

        [[nodiscard]] int get_id() const noexcept { return m_id; }

    private:
        static int generate_id() noexcept;

    private:
        SOCKET m_sock;
        int m_id;
        static std::atomic<int> idGen;

    public:
        char ring_buffer[buffer_size]{};
        int32_t cur_size = 0;
    };
}

#pragma once
#include <functional>
#include <sstream>
#include "Session.hpp"

namespace iocp_socket
{
    void session::receive_from_buffer(const CHAR buffer[], const DWORD rece_length,
                                      const std::function<void(std::string&)>& call_back)
    {
        memcpy(ring_buffer + cur_size, buffer, rece_length);
        cur_size += rece_length;
        int offset = 0;
        while (cur_size - offset >= sizeof(int32_t))
        {
            // 处理长度
            int length = ntohl(*reinterpret_cast<const int32_t*>(ring_buffer + offset));
            if (cur_size - offset < length)
            {
                break;
            }
            std::cout << "Parsed length (from combined buffer): " << length << '\n';

            // 处理一个包
            std::ostringstream oss;
            oss << std::string( ring_buffer + offset + 4, length - 4);
            std::string sendStr = oss.str();

            if (call_back != nullptr)
            {
                call_back(sendStr);
            }

            offset += length;
        };

        if (offset > 0)
        {
            if (cur_size - offset > 0)
            {
                memcpy(ring_buffer, ring_buffer + offset,
                       cur_size - offset);
            }

            cur_size -= offset;
        }
    }

    session::~session()
    {
        if (m_sock != INVALID_SOCKET)
        {
            closesocket(m_sock);
        }
        std::cout << "Session 销毁 : " << m_id << "\n";
    }

    void session::send_async(const std::string& message) const
    {
        send_async(message.data(), message.size());
    }

    void session::send_async(const char stream[], const size_t length) const
    {
        const auto overlapped = new v_overlapped(m_sock, IO_SEND);
        memcpy(overlapped->m_buf.buf, stream, length);

        constexpr DWORD flags = 0;
        overlapped->m_buf.len = length;
        if (WSASend(m_sock, &overlapped->m_buf, 1, nullptr, flags, overlapped, nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                throw std::runtime_error("WSASend failed");
            }
        }
    }

    int session::send_sync(const std::vector<char>& message) const
    {
        int code = send(m_sock, message.data(), message.size(), 0);
        return code;
    }

    int session::generate_id() noexcept
    {
        static std::atomic<int> counter(0); // 静态原子计数器
        return counter.fetch_add(1, std::memory_order_relaxed);
    }
}

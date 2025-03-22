#pragma once
#include <sstream>
#include "Session.hpp"

Session::~Session()
{
    if (m_sock != INVALID_SOCKET)
    {
        closesocket(m_sock);
    }
    std::cout << "Session 销毁 : " << m_id << "\n";
}

void Session::SendAsync(std::string s)
{
    VOverlapped *overlapped = new VOverlapped(m_sock, IO_SEND);
    memcpy(overlapped->m_buf.buf, s.c_str(), s.length());

    DWORD flags = 0;
    if (WSASend(m_sock, &overlapped->m_buf, 1, nullptr, flags, overlapped, nullptr) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            throw std::runtime_error("WSASend failed");
        }
    }
}

int Session::GenerateID() noexcept
{
    static std::atomic<int> counter(0); // 静态原子计数器
    return counter.fetch_add(1, std::memory_order_relaxed);
}

#include "v_overlapped.hpp"
#include <iostream>

namespace iocp_server
{
    v_overlapped::v_overlapped(SOCKET sock, IO_EVENT event)
    {
        memset(this, 0, sizeof(v_overlapped));
        m_sockClient = sock;
        m_buf.buf = m_btBuf;
        m_buf.len = sizeof(m_btBuf);
        m_dwBytesReceived = 0;
        m_dwFlag = 0;
        m_event = event;

        // std::cout << "VOcerlapped\n";
    }

    v_overlapped::~v_overlapped()
    {
        // std::cout << "~VOcerlapped\n";
    }

    void v_overlapped::reset()
    {
        Internal = 0;
        InternalHigh = 0;
        Offset = 0;
        OffsetHigh = 0;
        hEvent = nullptr;

        m_dwBytesReceived = 0;
        m_dwFlag = 0;
    }
}

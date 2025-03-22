#include "VOverlapped.hpp"
#include <iostream>

VOverlapped::VOverlapped(SOCKET sock, IO_EVENT event)
{
    memset(this, 0, sizeof(VOverlapped));
    m_sockClient = sock;
    m_buf.buf = m_btBuf;
    m_buf.len = sizeof(m_btBuf);
    m_dwBytesRecved = 0;
    m_dwFlag = 0;
    m_event = event;

    // std::cout << "VOcerlapped\n";
}

VOverlapped::~VOverlapped()
{
    // std::cout << "~VOcerlapped\n";
}

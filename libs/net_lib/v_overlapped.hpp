#pragma once

#define WIN32_LEAN_AND_MEAN
// #define FD_SETSIZE 128
#include <Winsock2.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

#include <memory>
#include <Mswsock.h>
#pragma comment(lib, "Mswsock.lib")
#include "io_event.hpp"

namespace iocp_server
{
    class v_overlapped : public OVERLAPPED
    {
    public:
        v_overlapped(SOCKET sock, IO_EVENT event);
        ~v_overlapped();
        void reset();

        IO_EVENT m_event;
        SOCKET m_sockClient;
        WSABUF m_buf;
        CHAR m_btBuf[MAXBYTE];
        DWORD m_dwBytesReceived;
        DWORD m_dwFlag;
    };
}

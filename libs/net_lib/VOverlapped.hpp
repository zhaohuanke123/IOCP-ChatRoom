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

class VOverlapped : public OVERLAPPED
{
public:
    VOverlapped(SOCKET sock, IO_EVENT event);
    ~VOverlapped();

public:
    IO_EVENT m_event;
    SOCKET m_sockClient;
    WSABUF m_buf;
    CHAR m_btBuf[MAXBYTE];
    DWORD m_dwBytesRecved;
    DWORD m_dwFlag;
    // std::shared_ptr<Session> m_session;
    // void* m_userData;

public:
    // void SetUserData(void* data) { m_userData = data; }
    // void* GetUserData() const { return m_userData; }
    
    void Reset() {
        Internal = 0;
        InternalHigh = 0;
        Offset = 0;
        OffsetHigh = 0;
        hEvent = nullptr;
        
        m_dwBytesRecved = 0;
        m_dwFlag = 0;
    }
};
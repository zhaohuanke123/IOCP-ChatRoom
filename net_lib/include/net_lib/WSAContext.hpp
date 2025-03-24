#pragma once
#include <winsock2.h>
#include <stdexcept>

class WSAContext {
public:
    WSAContext() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
            WSACleanup();
            throw std::runtime_error("Invalid Winsock version");
        }
    }

    ~WSAContext() noexcept {
        WSACleanup();
    }

    // 禁止拷贝和赋值
    WSAContext(const WSAContext&) = delete;
    WSAContext& operator=(const WSAContext&) = delete;
}; 
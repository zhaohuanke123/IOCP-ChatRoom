#pragma once
#include <winsock2.h>
#include <stdexcept>

/// @brief Winsock库上下文管理器
/// @details 封装WSAStartup/WSACleanup生命周期管理，确保资源自动回收
/// @remark 采用RAII模式实现异常安全，强制使用Winsock 2.2版本
class WSAContext {
public:
    /// @brief 初始化Winsock 2.2库
    /// @throws std::runtime_error 初始化失败或版本不匹配时抛出
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

    /// @brief 自动清理Winsock资源
    /// @note 声明noexcept保证异常安全，避免资源泄漏
    ~WSAContext() noexcept {
        WSACleanup();
    }

    WSAContext(const WSAContext&) = delete;           
    WSAContext& operator=(const WSAContext&) = delete; 
};

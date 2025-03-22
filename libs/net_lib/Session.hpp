#pragma once
#include "VOverlapped.hpp"
#include <memory>
#include <iostream>

class Session
{
    static const int BUFFERSIZE = 2048;

public:
    Session() : m_sock(INVALID_SOCKET)
    {
    }

    explicit Session(SOCKET sock) : m_sock(sock), m_id(GenerateID())
    {
        std::cout << "Session 创建 : " << m_id << "\n";
    }

    Session(const Session &) = delete;
    Session &operator=(const Session &) = delete;
    Session(Session &&other) noexcept
        : m_sock(std::exchange(other.m_sock, INVALID_SOCKET)),
          m_id(other.m_id) // 移动后保留原ID
    {
    }




    ~Session();
    void SendAsync(std::string s);

    int GetID() const noexcept { return m_id; }

private:
    static int GenerateID() noexcept;

private:
    SOCKET m_sock;
    int m_id;
    static std::atomic<int> idGen;

public:
    char ringBuffer[BUFFERSIZE];
    int32_t curSize = 0;
};

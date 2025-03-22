#pragma once
#include <winsock2.h>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "VOverlapped.hpp"
#include "Session.hpp"

class IOCPServer
{
public:
    IOCPServer(const char *ip, u_short port);
    ~IOCPServer();

    void Start();

private:
    void InitializeSocket();
    void SetupIOCP();
    void PostAccept();
    void PostRecv(SOCKET clientSocket);
    void WorkerThread();
    void HandleDisConnect(VOverlapped *vOver, DWORD bytesTransferred);

    std::mutex m_connMutex;
    std::unordered_map<SOCKET, std::shared_ptr<Session>,
                       std::hash<SOCKET>, std::equal_to<SOCKET>>
        m_activeConnections;
    SOCKET m_listenSocket;
    HANDLE m_hIocp;
    sockaddr_in m_serverAddr;
    bool m_running;
};
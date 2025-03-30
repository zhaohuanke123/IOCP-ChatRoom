#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Winsock2.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

#include <Mswsock.h>

#include <memory>
#pragma comment(lib, "Mswsock.lib")
#include <net_lib/io_event.hpp>

namespace iocp_socket {

/// @brief IOCP重叠IO结构扩展类
/// @details 继承Windows OVERLAPPED结构，封装异步IO操作上下文数据
/// @remark 集成WSABUF缓冲区管理，支持IOCP事件类型标识与套接字关联
class v_overlapped : public OVERLAPPED {
 public:
  /// @brief 构造异步IO操作上下文
  /// @param sock 关联的客户端套接字
  /// @param event 绑定IO事件类型（读/写/接受/断开）
  v_overlapped(SOCKET sock, io_event event);

  ~v_overlapped();

  /// @brief 重置上下文状态
  /// @note 清空接收缓冲区，复位字节计数与标志位
  void reset();

  // IO事件类型标识枚举值
  io_event m_event;
  // 关联的客户端套接字描述符
  SOCKET m_sockClient;
  // Winsock异步IO缓冲区结构
  WSABUF m_buf;
  // 固定大小数据缓冲区（最大字节数）
  CHAR m_btBuf[MAXBYTE];
  // 实际接收字节数计数器
  DWORD m_dwBytesReceived;
  // WSARecv等操作标志位存储
  DWORD m_dwFlag;
};

}  // namespace iocp_socket

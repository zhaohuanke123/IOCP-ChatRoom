#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>

#include <functional>
#include <iostream>
#include <net_lib/message_type.hpp>
#include <net_lib/v_overlapped.hpp>
#include <string>

namespace iocp_socket {

/// @brief 网络会话管理类
/// @details 封装Socket连接生命周期管理，提供同步/异步消息发送接口
class session {
  // 环形缓冲区容量
  static constexpr int buffer_size = 2048;

 public:
  /// @brief 默认构造无效会话对象
  session() : m_sock(INVALID_SOCKET), m_id(-1) {}

  /// @brief 显式构造有效会话对象
  /// @param sock 已建立的Socket连接句柄
  explicit session(const SOCKET sock) : m_sock(sock), m_id(generate_id()) {
    std::cout << "Session 创建 : " << m_id << "\n";
  }

  session(const session &) = delete;
  session &operator=(const session &) = delete;

  /// @brief 移动构造实现资源转移
  /// @param other 源会话对象
  /// @note 保留原会话ID，转移Socket所有权
  session(session &&other) noexcept
      : m_sock(std::exchange(other.m_sock, INVALID_SOCKET)), m_id(other.m_id) {}

  ~session();

  /// @brief 获取Socket句柄
  [[nodiscard]] SOCKET get_socket() const { return m_sock; }

  /// @brief 获取远端终端地址信息
  /// @return "IP:Port"格式字符串
  /// @throws 可能抛出Windows Socket API错误
  [[nodiscard]] std::string get_remote_endpoint() const {
    sockaddr_in sa{};
    int len = sizeof(sa);
    if (getpeername(m_sock, reinterpret_cast<sockaddr *>(&sa), &len) ==
        SOCKET_ERROR) {
      std::cerr << "getpeername failed with error: " << WSAGetLastError();
    }
    return std::string(inet_ntoa(sa.sin_addr)) + ":" +
           std::to_string(ntohs(sa.sin_port));
  }

  /// @brief 异步发送模板消息
  /// @tparam T 需实现to_json()序列化方法的消息类型
  /// @param mt 消息类型枚举值
  /// @param message 待发送消息对象
  template <typename T>
  void send_async(message_type mt, const T &message) const {
    auto bytes = serialize_data(mt, message.to_json());
    send_async(bytes.data(), bytes.size());
  }

  /// @brief 字符串消息异步发送重载
  void send_async(const std::string &message) const;

  /// @brief C风格字符串异步发送重载
  void send_async(const char *message) const;

  /// @brief 字节流异步发送重载
  void send_async(const std::vector<char> message) const;

  /// @brief 原始数据异步发送核心实现
  /// @param stream 数据缓冲区指针
  /// @param length 数据长度(字节)
  void session::send_async(const char stream[], const size_t length) const;

  /// @brief 同步发送字节流
  /// @return 成功发送字节数，错误返回SOCKET_ERROR
  int send_sync(const std::vector<char> &message) const;

  /// @brief 同步发送模板消息
  /// @return 成功发送字节数，错误返回SOCKET_ERROR
  template <class T>
  int send_sync(message_type mt, const T &msg) const {
    auto bytes = serialize_data(mt, msg.to_json());
    return send(m_sock, bytes.data(), bytes.size(), 0);
  }

  /// @brief 获取会话唯一标识符
  [[nodiscard]] int get_id() const noexcept { return m_id; }

 private:
  /// @brief 生成递增会话ID
  static int generate_id() noexcept;

 private:
  // 唯一会话标识符
  int m_id;
  // ID生成原子计数器
  static std::atomic<int> idGen;
  // 底层Socket连接句柄
  SOCKET m_sock;

 public:
  // 接收数据缓冲区
  char ring_buffer[buffer_size]{};
  // 当前缓冲区有效数据长度
  int32_t cur_size = 0;
};

}  // namespace iocp_socket

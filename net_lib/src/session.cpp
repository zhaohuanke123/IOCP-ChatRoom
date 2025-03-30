#pragma once
#include <functional>
#include <net_lib\message_type.hpp>
#include <net_lib\session.hpp>
#include <sstream>

namespace iocp_socket {

session::~session() {
  if (m_sock != INVALID_SOCKET) {
    closesocket(m_sock);
  }
  std::cout << "Session 销毁 : " << m_id << "\n";
}

void session::send_async(const std::string &message) const {
  send_async(message.data(), message.size());
}


void session::send_async(const char *message) const {
  send_async(message, strlen(message));
}

void session::send_async(const std::vector<char> message) const {
  send_async(message.data(), message.size());
}

void session::send_async(const char stream[], const size_t length) const {
  const auto overlapped = new v_overlapped(m_sock, IO_SEND);
  memcpy(overlapped->m_buf.buf, stream, length);

  constexpr DWORD flags = 0;
  overlapped->m_buf.len = length;
  if (WSASend(m_sock, &overlapped->m_buf, 1, nullptr, flags, overlapped,
              nullptr) == SOCKET_ERROR) {
    if (WSAGetLastError() != WSA_IO_PENDING) {
      throw std::runtime_error("WSASend failed");
    }
  }
}

int session::send_sync(const std::vector<char> &message) const {
  int code = send(m_sock, message.data(), message.size(), 0);
  return code;
}

int session::generate_id() noexcept {
  // 静态原子计数器
  static std::atomic<int> counter(0);  
  return counter.fetch_add(1, std::memory_order_relaxed);
}
}  // namespace iocp_socket

#pragma once
#include <memory>
#include <net_lib/message_type.hpp>
#include <net_lib/package_dispatcher.hpp>
#include <net_lib/session.hpp>

namespace iocp_socket {

/// @brief 网络协议序列化/反序列化工具集
/// @details 实现基于长度前缀的二进制协议格式，支持消息类型嵌入与JSON负载序列化
/// @remark 采用环形缓冲区实现TCP流式数据的可靠解析，处理粘包/拆包问题
namespace {
/// @brief 协议序列化核心函数
/// @param mt 消息类型标识符
/// @param message JSON格式消息体
/// @return 符合二进制协议的字节序列
/// @throws nlohmann::json::exception JSON序列化失败时抛出
static std::vector<char> serialize_data(message_type mt, const json &message) {
  std::vector<char> buffer;
  auto str = message.dump();  // 执行JSON序列化操作

  // 协议头组成：总长度(4B) + 消息类型(4B)
  // 插入长度字段
  int32_t length = str.length() + 8;
  int32_t net_length = htonl(length);
  buffer.insert(buffer.end(), reinterpret_cast<char *>(&net_length),
                reinterpret_cast<char *>(&net_length) + sizeof(int32_t));

  // 插入消息类型字段
  int32_t net_message_type = htonl(static_cast<int32_t>(mt));
  buffer.insert(buffer.end(), reinterpret_cast<char *>(&net_message_type),
                reinterpret_cast<char *>(&net_message_type) + sizeof(int32_t));

  // 追加JSON序列化结果
  buffer.insert(buffer.end(), str.begin(), str.end());
  return buffer;
}

// 长度字段字节数
static constexpr int LENGTH_SIZE = sizeof(int32_t);
// 协议头总长度
static constexpr int HEADER_SIZE = LENGTH_SIZE + sizeof(int32_t);

/// @brief 流式数据解析入口
/// @param sendSession 关联会话上下文（含缓冲区）
/// @param buffer 接收缓冲区指针
/// @param rece_length 本次接收数据长度
/// @param dispatcher 消息分发器引用
static void receive_from_buffer(const std::shared_ptr<session> &sendSession,
                                const CHAR *buffer, DWORD rece_length,
                                package_dispatcher dispatcher) {
  char *ring_buffer = sendSession->ring_buffer;
  int &cur_size = sendSession->cur_size;

  // 数据先放到缓冲区，方便同一处理
  memcpy(ring_buffer + cur_size, buffer, rece_length);
  cur_size += rece_length;
  // 当前解析位置偏移量
  int offset = 0;

  // 流式解析主循环（处理可能存在的多个完整包）
  while (cur_size - offset >= HEADER_SIZE) {
    // 解析网络字节序长度
    int length =
        ntohl(*reinterpret_cast<const int32_t *>(ring_buffer + offset));
    // 剩余数据不足时退出
    if (cur_size - offset < length) {
      break;
    }

    // 解析消息类型
    int mt = ntohl(
        *reinterpret_cast<const int32_t *>(ring_buffer + offset + LENGTH_SIZE));
    auto type = static_cast<message_type>(mt);
    // 提取JSON
    std::string payload(ring_buffer + offset + HEADER_SIZE,
                        length - HEADER_SIZE);

    // 消息分发
    dispatcher.dispatch_message(sendSession, type, payload);
    // 移动解析偏移量
    offset += length;
  };

  // 处理剩余数据（可能存在的半包）
  if (offset > 0) {
    if (cur_size - offset > 0) {
      {
        // 数据前移操作
        memcpy(ring_buffer, ring_buffer + offset, cur_size - offset);
      }
    }
    // 更新有效数据长度
    cur_size -= offset;
  }
}
}  // namespace
}  // namespace iocp_socket

#pragma once
#include <any>
#include <functional>
#include <net_lib/message_type.hpp>
#include <net_lib/session.hpp>
#include <string>
#include <unordered_map>

namespace iocp_socket {
class package_dispatcher {
 public:
  /// @brief 注册消息类型处理模板函数
  /// @tparam T 消息数据类型（需支持from_json反序列化）
  /// @param mt 消息类型枚举值
  /// @param handler 消息处理回调函数
  /// @throws std::bad_any_cast 当类型转换失败时抛出
  template <typename T>
  void register_message_handler(
      message_type mt,
      std::function<void(std::shared_ptr<session> &send_session, const T &)>
          handler) {
    g_message_map[mt] = [handler](std::shared_ptr<session> &send_session,
                                  const std::any &data) {
      handler(send_session, std::any_cast<const T &>(data));
    };
  }

  /// @brief 消息分发主逻辑
  /// @param send_session 发送方会话上下文
  /// @param mt 消息类型标识符
  /// @param message JSON格式原始消息数据
  void dispatch_message(std::shared_ptr<session> send_session, message_type mt,
                        const std::string &message) {
    std::cout << "收到消息" << static_cast<int>(mt) << "\n";

    /// 1. 根据mt选择消息解析器
    message_type type = static_cast<message_type>(mt);
    auto handler = get_message_handler(type);
    switch (type) {
      case message_type::login: {
        /// 2. 执行JSON反序列化
        auto _message = login_message::from_json(json::parse(message));
        /// 3. 通过安全调用机制触发处理器
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::enter_room: {
        auto _message = enter_room::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::get_rooms_request: {
        auto _message = get_rooms_request::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::get_rooms_response: {
        auto _message = get_rooms_response::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::message: {
        auto _message = message::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::send_message: {
        auto _message = send_message::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::get_users_request: {
        auto _message = get_users_request::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::get_users_response: {
        auto _message = get_users_response::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::create_room: {
        auto _message = create_room::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::leave_room: {
        auto _message = leave_room::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
    }
  }

 private:
  /// @brief 获取消息类型对应的处理函数
  /// @param mt 目标消息类型
  /// @return 注册的处理函数（未注册时返回nullptr）
  auto get_message_handler(message_type mt) const
      -> std::function<void(std::shared_ptr<session> &,
                            const std::any &)> const {
    auto it = g_message_map.find(mt);
    if (it != g_message_map.end()) {
      return it->second;
    }
    return nullptr;
  }

  /// @brief 安全调用包装器
  /// @tparam T 消息数据类型
  /// @param handler 目标处理函数
  /// @param send_session 关联会话对象
  /// @param data 类型化消息数据
  template <typename T>
  void safeInvoke(
      std::function<void(std::shared_ptr<session> &, const std::any &)> handler,
      std::shared_ptr<session> send_session, const T &data) {
    try {
      if (handler) {
        handler(send_session, data);
      }
    } catch (const std::exception &e) {
      std::cerr << "消息处理异常: " << e.what() << "\n";
    }
  }

  /// @brief 消息类型-处理函数映射表
  /// @key 消息类型枚举值
  /// @value 对应的消息处理函数（通过std::any实现类型擦除）
 private:
  std::unordered_map<message_type, std::function<void(std::shared_ptr<session>,
                                                      const std::any &)>>
      g_message_map;
};
}  // namespace iocp_socket

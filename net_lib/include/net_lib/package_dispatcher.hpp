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

  void dispatch_message(std::shared_ptr<session> send_session, message_type mt,
                        const std::string &message) {
    std::cout << "收到消息" << static_cast<int>(mt) << "\n";
    message_type type = static_cast<message_type>(mt);
    auto handler = get_message_handler(type);
    switch (type) {
      case message_type::login: {
        auto _message = login_message::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::enter_room: {
        auto _message = enter_room_message::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::get_room_list: {
        auto _message = get_room_list_message::from_json(json::parse(message));
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
        auto _message =
            get_users_request_message::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
      case message_type::get_users_response: {
        auto _message =
            get_users_response_message::from_json(json::parse(message));
        safeInvoke(handler, send_session, _message);
        break;
      }
    }
  }

 private:
  auto get_message_handler(message_type mt) const
      -> std::function<void(std::shared_ptr<session> &,
                            const std::any &)> const {
    auto it = g_message_map.find(mt);
    if (it != g_message_map.end()) {
      return it->second;
    }
    return nullptr;
  }

  template <typename T>
  void safeInvoke(
      std::function<void(std::shared_ptr<session> &, const std::any &)> handler,
      std::shared_ptr<session> send_session, const T &data) {
    if (handler) {
      handler(send_session, data);
    } else {
      std::cout << "没有找到对应的消息处理器" << "\n";
    }
  }

 private:
  std::unordered_map<message_type, std::function<void(std::shared_ptr<session>,
                                                      const std::any &)>>
      g_message_map;
};
}  // namespace iocp_socket

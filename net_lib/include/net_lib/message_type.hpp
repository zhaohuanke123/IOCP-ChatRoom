#pragma once
#include <cstdint>
#include <net_lib/json.hpp>
#include <string>
#include <vector>
using json = nlohmann::json;

namespace iocp_socket {
enum class message_type {
  login,          // 客户端登录
  enter_room,     // 客户端进入房间
  get_rooms_request,  // 客户端请求房间列表
  get_rooms_response, // 服务器响应房间列表
  send_message,   // 客户端发送消息
  message,        // 服务器转发消息
  leave_room, // 客户端离开房间
  get_users_request,   // 客户端请求用户列表
  get_users_response,  // 服务器响应用户列表
  create_room,         // 客户端创建房间
  message_count,
};

struct login_message {
  std::string username;
  std::string password;

  json to_json() const {
    return json{{"username", username}, {"password", password}};
  }

  static login_message from_json(const json &j) {
    login_message p;
    p.username = j.at("username").get<std::string>();
    p.password = j.at("password").get<std::string>();
    return p;
  }
};

struct enter_room {
   std::string room_name;

  json to_json() const {
    return json{{"room_name", room_name}};
  }

  static enter_room from_json(const json &j) {
    enter_room p;
    p.room_name = j.at("room_name").get<std::string>();
    return p;
  }
};

enum class send_message_type { someone, room };

struct send_message {
  std::string content;
  std::string receiver;
  send_message_type type;

  json to_json() const {
    return json{ {"content", content}, {"receiver", receiver}, {"type", static_cast<int>(type)}};
  }

  static send_message from_json(const json &j) {
    send_message p;
    p.content = j.at("content").get<std::string>();
    p.receiver = j.at("receiver").get<std::string>();
    p.type = static_cast<send_message_type>(j.at("type").get<int>());
    return p;
  }
};

struct message {
  std::string sender;
  std::string content;
  json to_json() const {
    return json{{"sender", sender}, {"content", content}};
  }
  static message from_json(const json &j) {
    message p;
    p.sender = j.at("sender").get<std::string>();
    p.content = j.at("content").get<std::string>();
    return p;
  }
};

struct get_users_request {
  json to_json() const { return json{}; }

  static get_users_request from_json(const json &j) {
    get_users_request p;
    return p;
  }
};

struct get_users_response {
  std::vector<std::string> user_list;

  json to_json() const { return json{{"user_list", user_list}}; }

  static get_users_response from_json(const json &j) {
    get_users_response p;
    p.user_list = j.at("user_list").get<std::vector<std::string>>();
    return p;
  }
};

struct create_room {
  std::string room_name;
  json to_json() const { return json{{"room_name", room_name}}; }

  static create_room from_json(const json &j) {
    create_room p;
    p.room_name = j.at("room_name").get<std::string>();
    return p;
  }
};

struct leave_room {
  std::string room_name;
  json to_json() const { return json{{"room_name", room_name}}; }

  static leave_room from_json(const json &j) {
    leave_room p;
    p.room_name = j.at("room_name").get<std::string>();
    return p;
  } 
};

struct get_rooms_request {
  json to_json() const { return json{}; }

  static get_rooms_request from_json(const json &j) {
    get_rooms_request p;
    return p;    
  }
};

struct get_rooms_response {
  std::vector<std::string> room_list;
  json to_json() const { return json{{"room_list", room_list}};}
  static get_rooms_response from_json(const json &j) {
    get_rooms_response p;
    p.room_list = j.at("room_list").get<std::vector<std::string>>();
    return p; 
  }  
};

}  // namespace iocp_socket

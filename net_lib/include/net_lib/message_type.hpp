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
  get_room_list,  // 客户端请求房间列表 没用
  send_message,   // 客户端发送消息
  message,        // 服务器转发消息
  leave_room,
  message_count,
  get_users_request,   // 客户端请求用户列表
  get_users_response,  // 服务器响应用户列表
  create_room,         // 客户端创建房间
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

struct get_room_list_message {
  std::vector<std::string> room_list;

  json to_json() const { return json{{"room_list", room_list}}; }

  static get_room_list_message from_json(const json &j) {
    get_room_list_message p;
    p.room_list = j.at("room_list").get<std::vector<std::string>>();
    return p;
  }
};

struct enter_room_message {
  std::string room_name;

  json to_json() const { return json{{"room_name", room_name}}; }

  static enter_room_message from_json(const json &j) {
    enter_room_message p;
    p.room_name = j.at("room_name").get<std::string>();
    return p;
  }
};

struct send_message {
  std::string content;
  std::string receiver;

  json to_json() const {
    return json{{"content", content}, {"receiver", receiver}};
  }

  static send_message from_json(const json &j) {
    send_message p;
    p.content = j.at("content").get<std::string>();
    p.receiver = j.at("receiver").get<std::string>();
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
}  // namespace iocp_socket

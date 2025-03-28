#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <net_lib/json.hpp>
using json = nlohmann::json;

namespace iocp_socket
{
   enum message_type
   {
      login,
      enter_room,
      get_room_list,
      send_message,
      message,
      leave_room,
      message_count,
      get_users_request,
      get_users_response,
   };

   struct login_message
   {
      std::string username;
      std::string password;

      json to_json() const
      {
         return json{{"username", username}, {"password", password}};
      }

      static login_message from_json(const json &j)
      {
         login_message p;
         p.username = j.at("username").get<std::string>();
         p.password = j.at("password").get<std::string>();
         return p;
      }
   };

   struct get_room_list_message
   {
      std::vector<std::string> room_list;

      json to_json() const
      {
         return json{{"room_list", room_list}};
      }

      static get_room_list_message from_json(const json &j)
      {
         get_room_list_message p;
         p.room_list = j.at("room_list").get<std::vector<std::string>>();
         return p;
      }
   };

   struct enter_room_message
   {
      std::string room_name;

      json to_json() const
      {
         return json{{"room_name", room_name}};
      }

      static enter_room_message from_json(const json &j)
      {
         enter_room_message p;
         p.room_name = j.at("room_name").get<std::string>();
         return p;
      }
   };

   struct send_message
   {
      std::string content; 

      json to_json() const
      {
         return json{{"content", content}};
      }

      static send_message from_json(const json &j)
      {
         send_message p;
         p.content = j.at("content").get<std::string>();
         return p;
      }
   };

   struct message
   {
      std::string sender;
      std::string content;
      json to_json() const
      {
         return json{{"sender", sender}, {"content", content}};
     
      }
      static message from_json(const json &j)
      {
         message p;
         p.sender = j.at("sender").get<std::string>();
         p.content = j.at("content").get<std::string>();
         return p;
      }
   };

   struct get_users_request_message
   {
      json to_json() const
      {
         return json{};
      }

      static get_users_request_message from_json(const json &j)
      {
         get_users_request_message p;
         return p;
      }
   };

   struct  get_users_response_message
   {
      std::vector<std::string> user_list;

      json to_json() const
      {
         return json{{"user_list", user_list}};
      }

      static get_users_response_message from_json(const json &j)
      {
         get_users_response_message p;
         p.user_list = j.at("user_list").get<std::vector<std::string>>();
         return p;
      }
   };
} // namespace iocp_socket

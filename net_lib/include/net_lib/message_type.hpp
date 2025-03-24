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
      leave_room,
      message_count,
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
   };

   struct enter_room_message
   {
      std::string room_name;
   };

} // namespace iocp_socket

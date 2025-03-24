#pragma once
#include <any>
#include <unordered_map>
#include <memory>
#include <string>
#include <functional>
#include <net_lib/message_type.hpp>
#include <iostream>

namespace iocp_socket
{
    class package_dispatcher
    {
    public:
        template <typename T>
        void register_message_handler(message_type mt, std::function<void(const T &)> handler)
        {
            g_message_map[mt] = [handler](const std::any &data)
            {
                handler(std::any_cast<const T &>(data));
            };
        }

        void dispatch_message(int mt, const std::string &message)
        {
            message_type type = static_cast<message_type>(mt);
            auto handler = get_message_handler(type);
            switch (type)
            {
            case message_type::login:
            {
                auto login_message = login_message::from_json(json::parse(message));
                safeInvoke(handler, login_message);
                break;
            }
            case message_type::enter_room:
            {
                auto enter_room_message = enter_room_message::from_json(json::parse(message));
                safeInvoke(handler, enter_room_message);
                break;
            }
            case message_type::get_room_list:
            {
                auto get_room_list_message = get_room_list_message::from_json(json::parse(message));
                safeInvoke(handler, get_room_list_message);
                break;
            }
            case message_type::send_message:
            {
                auto send_message = send_message::from_json(json::parse(message));
                safeInvoke(handler, send_message);
                break;
            }
            // case message_type::message:
            // {
            //     auto message = message::from_json(json::parse(message));
            //     safeInvoke(handler, message);
            //     break;
            // }
            }
        }

    private:
        auto get_message_handler(message_type mt) const -> std::function<void(const std::any &)> const
        {
            auto it = g_message_map.find(mt);
            if (it != g_message_map.end())
            {
                return it->second;
            }
            return nullptr;
        }

        template <typename T>
        void safeInvoke(std::function<void(const std::any &)> handler, const T &data)
        {
            if (handler)
            {
                handler(data);
            }
        }

    private:
        std::unordered_map<message_type, std::function<void(const std::any &)>> g_message_map;
    };
}

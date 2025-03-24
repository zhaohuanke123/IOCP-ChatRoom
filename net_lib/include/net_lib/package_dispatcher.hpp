#pragma once
#include <mutex>
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
        std::mutex g_mutex;
        std::unordered_map<message_type, std::function<void(const std::string&)>> g_message_map;

        void register_message_handler(message_type mt, std::function<void(const std::string &)> handler)
        {
            g_message_map[mt] = handler;
            std::cout << "register : " << mt << std::endl;
        }

        void dispatch_message(int mt, const std::string &message)
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            message_type type = (message_type)mt;

            // void *mes = nullptr;

            // switch (type)
            // {
            // case message_type::login:
            //     mes = &login_message::from_json(json::parse(message));
            //     break;
            // default:
            //     break;
            // }

            auto it = g_message_map.find(type);
            std::cout << g_message_map.size() << std::endl;
            if (it != g_message_map.end())
            {
                it->second(message);
            }
        }
    };
} // namespace iocp_socket

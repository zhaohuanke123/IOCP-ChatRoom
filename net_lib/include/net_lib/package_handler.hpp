#pragma once
#include <net_lib/Server.hpp>
#include <net_lib/message_type.hpp>

namespace iocp_socket
{
    // class package_handler
    // {
    // public:
    static std::vector<char> serialize_data(const std::string &message)
    {
        std::vector<char> buffer;
        int32_t length = message.length() + 4;

        // 1. 序列化 int32 (转换为网络字节序)
        int32_t net_length = htonl(length);
        buffer.insert(buffer.end(),
                      reinterpret_cast<char *>(&net_length),
                      reinterpret_cast<char *>(&net_length) + sizeof(int32_t));

        // 2. 序列化 string 内容
        buffer.insert(buffer.end(), message.begin(), message.end());

        return buffer;
    }

    static std::vector<char> serialize_data(message_type mt, const json &message)
    {
        std::vector<char> buffer;
        auto str = message.dump();

        int32_t length = str.length() + 8;
        // 1. 序列化 int32 (转换为网络字节序)
        int32_t net_length = htonl(length);
        buffer.insert(buffer.end(),
                      reinterpret_cast<char *>(&net_length),
                      reinterpret_cast<char *>(&net_length) + sizeof(int32_t));

        // 2. 序列化 message_type (转换为网络字节序)
        int32_t net_message_type = htonl(static_cast<int32_t>(mt));
        buffer.insert(buffer.end(),
                      reinterpret_cast<char *>(&net_message_type),
                      reinterpret_cast<char *>(&net_message_type) + sizeof(int32_t));

        // 3. 序列化 string 内容
        buffer.insert(buffer.end(), str.begin(), str.end());

        return buffer;
    }
    // };
}

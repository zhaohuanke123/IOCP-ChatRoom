#pragma once
#include <net_lib/server.hpp>
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

    static constexpr int LENGTH_SIZE = sizeof(int32_t);
    static constexpr int HEADER_SIZE = LENGTH_SIZE + sizeof(int32_t);

    static void receive_from_buffer(const shared_ptr<session> &sendSession, const CHAR *buffer, const DWORD rece_length, package_dispatcher dispatcher)
    {
        char *ring_buffer = sendSession->ring_buffer;
        int &cur_size = sendSession->cur_size;
        memcpy(ring_buffer + cur_size, buffer, rece_length);
        cur_size += rece_length;
        int offset = 0;
        while (cur_size - offset >= HEADER_SIZE)
        {
            // 处理长度
            int length = ntohl(*reinterpret_cast<const int32_t *>(ring_buffer + offset));
            if (cur_size - offset < length)
            {
                break;
            }
            std::cout << "Parsed length (from combined buffer): " << length << '\n';

            int mt = ntohl(*reinterpret_cast<const int32_t *>(ring_buffer + offset + LENGTH_SIZE));
            auto type = (message_type)mt;
            dispatcher.dispatch_message(sendSession, type, std::string(ring_buffer + offset + HEADER_SIZE, length - HEADER_SIZE));

            offset += length;
        };

        if (offset > 0)
        {
            if (cur_size - offset > 0)
            {
                memcpy(ring_buffer, ring_buffer + offset,
                       cur_size - offset);
            }

            cur_size -= offset;
        }
    }

    // };
}

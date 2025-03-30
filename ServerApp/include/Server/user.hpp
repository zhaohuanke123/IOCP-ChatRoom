#ifndef USER_H
#define USER_H
#include <string>
#include <memory>
#include <net_lib/session.hpp>
#include <net_lib/message_type.hpp>

namespace iocp_socket
{
    using std::string;
    using std::shared_ptr;

    class user
    {
    public:
        user(const string &name, const shared_ptr<session> &session);

        ~user();

        string get_name()
        {
            return name;
        }

        void send_message(const string& message) const
        {
            session->send_async(message);
        }

        template <typename T> void send_message(message_type mt, const T &msg) const
        {
            session->send_async(mt, msg);
        }

    private:
        string name;
        shared_ptr<session> session;
    };
} // iocp_socker

#endif //USER_H

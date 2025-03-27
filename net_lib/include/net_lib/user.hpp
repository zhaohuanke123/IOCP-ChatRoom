#ifndef USER_H
#define USER_H
#include <string>
#include <net_lib/session.hpp>

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

    private:
        string name;
        shared_ptr<session> session;
    };
} // iocp_socker

#endif //USER_H

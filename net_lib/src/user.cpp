//
// Created by zhaohuanke on 2025/3/27.
//

#include <net_lib/user.hpp>

namespace iocp_socket
{
    user::user(const string &name, const shared_ptr<iocp_socket::session> &session)
    {
        this->name = name;
        this->session = session;
    }

    user::~user()
    {
    }
} // iocp_socker

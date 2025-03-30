#include <Server/user.hpp>

namespace iocp_socket {
user::user(const string &name,
           const shared_ptr<iocp_socket::session> &session) {
  this->name = name;
  this->session = session;
}

user::~user() {}
}  // namespace iocp_socket

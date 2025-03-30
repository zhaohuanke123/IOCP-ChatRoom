#pragma once

#include <winsock2.h>

#include <Server/room.hpp>
#include <Server/user.hpp>
#include <memory>
#include <mutex>
#include <net_lib/Session.hpp>
#include <net_lib/message_type.hpp>
#include <net_lib/package_dispatcher.hpp>
#include <net_lib/v_overlapped.hpp>
#include <shared_mutex>
#include <unordered_map>
#include <utility>

namespace iocp_socket {

/// @brief IOCP服务端核心类
/// @details 实现基于Windows
/// IOCP模型的TCP服务器，管理网络连接、消息分发和房间系统
class server {
 public:
  /// @brief 构造函数初始化服务器地址
  /// @param ip 监听IP地址（IPv4格式）
  /// @param port 监听端口号
  server(const char *ip, u_short port);

  /// @brief 析构函数释放系统资源
  ~server();

  /// @brief 启动IOCP服务核心逻辑
  /// @note 包含线程池初始化、监听socket激活等操作
  void start();

  /// @brief 获取服务器单例实例
  /// @return 服务端静态实例指针
  /// @remark 采用静态局部变量实现线程安全的单例
  static server *get_instance() {
    static server instance("127.0.0.1", 9527);
    return &instance;
  }

  /// @brief 获取指定socket对应的会话对象
  /// @param sock 客户端socket描述符
  /// @return 对应session的共享指针
  /// @warning 需确保sock存在于活跃连接表中
  shared_ptr<session> get_session(SOCKET sock) {
    return m_activeConnections[sock];
  }

 private:
  /// @brief 初始化网络套接字
  /// @throws runtime_error 当socket创建/绑定失败时抛出
  void initialize_socket();

  /// @brief 创建IOCP 完成端口
  /// @throws runtime_error 当创建完成端口失败时抛出
  void setup_iocp();

  /// @brief 注册消息类型与处理函数的映射
  /// @note 需在服务器启动前完成所有消息处理器的注册
  void register_message_handler();

  /// @brief 投递异步接受连接请求
  void post_accept();

  /// @brief 投递异步接收操作
  void post_recv(SOCKET client_socket);

  /// @brief IOCP工作线程入口函数
  /// @details 循环处理完成端口上的IO事件
  void worker_thread();

  /// @brief 处理客户端断开连接
  /// @param v_over 关联的重叠结构指针
  /// @param bytes_transferred 最后传输的字节数
  void handle_dis_connect(v_overlapped *v_over, DWORD bytes_transferred);

  // 房间系统管理接口 ===============
  /// @brief 创建新房间
  /// @param name 房间唯一标识名称
  /// @return 新建房间的共享指针
  shared_ptr<room> create_new_room(const string &name);

  /// @brief 删除指定房间
  /// @param room_name 目标房间名称
  /// @return 是否成功删除
  bool delete_room(const string &room_name);

  /// @brief 获取指定房间对象
  /// @param room_name 目标房间名称
  /// @return 房间共享指针（不存在时返回nullptr）
  shared_ptr<room> get_room(const string &room_name) const;

  /// @brief 获取所有房间列表
  /// @return 包含所有房间的共享指针集合
  std::vector<std::shared_ptr<room>> get_all_rooms() const;
  // ===============================

  /// @brief 用户表操作锁
  std::mutex m_usersMutex;

  /// @brief 活跃连接映射表
  /// @key 客户端socket描述符
  /// @value 对应的session对象
  std::unordered_map<SOCKET, std::shared_ptr<session>> m_activeConnections;

  /// @brief socket-用户名映射表
  /// @key 客户端socket描述符
  /// @value 已登录的用户名
  std::unordered_map<SOCKET, std::string> m_socket2Name;

  /// @brief 已认证用户表
  /// @key 用户名（唯一标识）
  /// @value 用户对象实体
  std::unordered_map<std::string, std::shared_ptr<user>> m_loginUsers;

  /// @brief 房间数据读写锁
  mutable std::shared_mutex m_rooms_mutex;

  /// @brief 房间存储容器
  /// @key 房间名称（唯一标识）
  /// @value 房间对象实体
  std::unordered_map<std::string, std::shared_ptr<room>> m_rooms;

  /// @brief 服务端监听socket
  SOCKET m_listenSocket;

  /// @brief IOCP完成端口句柄
  HANDLE m_hIocp;

  /// @brief 服务器地址
  sockaddr_in m_serverAddr;

  /// @brief 服务运行状态标志位
  bool m_running;

  /// @brief 网络消息分发器（静态成员）
  static package_dispatcher m_dispatcher;
};

}  // namespace iocp_socket

# IOCP-ChatRoom
基于IOCP 的 控制台聊天室

├── ClientApp/                # 客户端应用程序
├── ServerApp/                # 服务器端应用程序
│   ├── include/Server/       # 服务端头文件
│   │   ├── room.hpp          # 聊天室类定义
│   │   ├── server.hpp        # 服务器主逻辑
│   │   └── user.hpp          # 用户实体类
│   └── src/
│       ├── main.cpp          # 服务端入口
│       ├── room.cpp          # 房间管理实现
│       ├── server.cpp        # 服务器逻辑实现
│       └── user.cpp          # 用户行为实现
├── net_lib/                  # 网络通信核心库
│   ├── include/net_lib/      # 公共头文件
│   │   ├── io_event.hpp      # IO事件处理
│   │   ├── json.hpp          # JSON序列化
│   │   ├── message_type.hpp  # 消息类型枚举
│   │   ├── package_dispatcher.hpp  # 数据包分发器
│   │   ├── package_handler.hpp     # 数据包处理器
│   │   ├── session.hpp       # 会话连接管理
│   │   ├── v_overlapped.hpp  # 重叠IO结构
│   │   └── WSAContext.hpp    # Winsock上下文
│   └── src/
│       ├── package_dispatcher.cpp  # 消息路由实现
│       ├── session.cpp        # 会话生命周期管理
│       └── v_overlapped.cpp   # 异步IO操作
├── cmake_modules/            # CMake扩展模块
├── demo_app/                 # 示例应用
├── CMakeLists.txt            # 主构建配置
└── README.md                 # 项目说明文档
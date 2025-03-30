#pragma once

/// @brief 异步I/O操作类型标识枚举
/// @details 与IOCP完成键(key)配合使用，用于区分网络操作类型
/// @remark 每种类型对应不同的重叠I/O处理逻辑
enum io_event
{
    // 连接接受事件：处理新客户端接入请求
    IO_ACCEPT,  
    // 数据接收事件：处理TCP数据到达通知
    IO_RECV,    
    // 数据发送事件：标识异步发送操作完成
    IO_SEND     
};
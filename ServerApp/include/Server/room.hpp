#ifndef CMAKEDEMO_ROOM_HPP
#define CMAKEDEMO_ROOM_HPP

#include <chrono>
#include <atomic>
#include <unordered_set>
#include <Server/user.hpp>
#include <shared_mutex>
#include <utility>

namespace iocp_socket {

/// @brief 在线聊天室管理类
/// @details 封装聊天室元数据及成员管理
class room {
public:
    /// @brief 显式构造函数
    /// @param name 聊天室名称标识
    /// @note 初始化时设置创建时间戳
    explicit room(std::string name)
        : m_name(std::move(name)),
          m_created_time(std::chrono::system_clock::now()) {}

    /// @brief 移动构造
    room(room&& other) noexcept
        : m_name(std::move(other.m_name)),
          m_members(std::move(other.m_members)),
          m_created_time(other.m_created_time) {}

    /// @brief 添加成员到聊天室
    /// @param user 待添加用户
    /// @return 添加成功返回true，人数已满或已存在返回false
    bool add_member(const std::shared_ptr<user>& user) {
        std::unique_lock lock(m_mutex);
        return m_members.insert(user).second;
    }

    /// @brief 从聊天室移除成员
    /// @param user 待移除用户
    /// @return 成功移除返回true，不存在返回false
    /// @note 写锁保护，线程安全
    bool remove_member(const std::shared_ptr<user>& user) {
        std::unique_lock lock(m_mutex);
        return m_members.erase(user) > 0;
    }

    /// @brief 检查用户是否在聊天室
    /// @return 存在返回true，否则false
    bool contains_member(const std::shared_ptr<user>& user) const {
        std::shared_lock lock(m_mutex);
        return m_members.count(user) > 0;
    }

    /// @brief 获取当前房间名称
    /// @return 名称字符串副本
    std::string name() const {
        std::shared_lock lock(m_mutex);
        return m_name;
    }

    /// @brief 线程安全迭代成员集合
    /// @tparam F 函数对象类型，需接受const shared_ptr<user>&参数
    /// @param func 回调函数，对每个成员执行只读操作
    /// @note 遍历期间持有读锁，禁止在回调中修改容器
    template<typename F>
    void for_each_member(F&& func) const {
        std::shared_lock lock(m_mutex);
        for (const auto& member : m_members) {
            func(member);
        }
    }

private:
    // 读写锁
    mutable std::shared_mutex m_mutex;    
     // 房间名称
    std::string m_name;                    
    // 用户集合哈希容器
    std::unordered_set<std::shared_ptr<user>> m_members; 
    // 房间创建时间戳（构造时初始化）
    const std::chrono::system_clock::time_point m_created_time;
};

} // namespace iocp_socket

#endif //CMAKEDEMO_ROOM_HPP

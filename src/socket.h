/**
 * @file socket.h
 * @author zhcs (zhanghength@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-21
 * 
 * @copyright Copyright (c) 2022 zhanghength@163.com
 * 
 * @pra 修改日志:
 * <table>
 * <tr><th>Date   <th>Version <th>Author  <th>Description
 * <tr><th>2022-04-21 <th>1.0 <th>zhcs    <th>create
 * </table>
 */
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <memory>
#include <sys/socket.h>
#include "nocopyable.h"

namespace zcs {

class Socket : public std::enable_shared_from_this<Socket>, Nocopyable {
public:
    typedef std::shared_ptr<Socket> ptr;
    typedef std::weak_ptr<Socket> weak_ptr;

    enum Type {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };

    enum Family {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX 
    };

    static Socket::ptr CreateTCP();
    static Socket::ptr CreateUDP();

    static Socket::ptr CreateTCPSocket();
    static Socket::ptr CreateUDPSocket();
    static Socket::ptr CreateTCPSocket6();
    static Socket::ptr CreateUDPSocket6();
    static Socket::ptr CreateTCPUnix();
    static Socket::ptr CreateUDPUnix();

    Socket(int family, int type, int protocol = 0);
    virtual ~Socket();
protected:
    int sock_;
    int family_;
    int type_;
    int protocol_;
    int is_connected_;
    

private:
};

} // namespace zcs


#endif //__SOCKET_H__
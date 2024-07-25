#ifndef MODBUSTCP_UNIX_H
#define MODBUSTCP_UNIX_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#include "Modbus_unix.h"

typedef int SOCKET;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

class ModbusTcpSocket
{
public:
    ModbusTcpSocket(SOCKET socket = INVALID_SOCKET) : m_socket(socket) {}

public:
    inline bool isValid() const { return m_socket != INVALID_SOCKET; }
    inline bool isInvalid() const { return m_socket == INVALID_SOCKET; }
    inline SOCKET socket() const { return m_socket; }
    inline void setSocket(SOCKET socket) { m_socket = socket; }
    inline ModbusTcpSocket &operator=(SOCKET socket) { setSocket(socket); return *this; }
    inline void setTimeout(uint32_t timeout) { setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)); }
    inline void setBlocking(bool block) { int flags = fcntl(m_socket, F_GETFL, 0); fcntl(m_socket, F_SETFL, block ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK)); }

public: // socket interface
    inline SOCKET create(int domain, int type, int protocol) { m_socket = ::socket(domain, type, protocol); return m_socket; }
    inline int getsockopt(int level, int optname, void *optval, socklen_t *optlen) { return ::getsockopt(m_socket, level, optname, optval, optlen); }
    inline int connect(const struct sockaddr *name, socklen_t namelen) { return ::connect(m_socket, name, namelen); }
    inline int bind(const struct sockaddr *name, socklen_t namelen) { return ::bind(m_socket, name, namelen); }
    inline int listen(int backlog) { return ::listen(m_socket, backlog); }
    inline SOCKET accept(struct sockaddr *addr, socklen_t *addrlen) { return ::accept(m_socket, addr, addrlen); }
    inline ssize_t send(const void *buf, size_t len, int flags) { return ::send(m_socket, buf, len, flags); }
    inline ssize_t recv(void *buf, size_t len, int flags) { return ::recv(m_socket, buf, len, flags); }
    inline void shutdown() { ::shutdown(m_socket, SHUT_RDWR); }
    inline void close() { ::close(m_socket); m_socket = INVALID_SOCKET; }

private:
    SOCKET m_socket;
};

#endif // MODBUSTCP_UNIX_H

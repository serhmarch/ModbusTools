#ifndef MODBUSTCP_WIN_H
#define MODBUSTCP_WIN_H

#include <WS2tcpip.h>

#include "Modbus_win.h"

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
    inline void setBlocking(bool block) { unsigned long ul = !block; ioctlsocket(m_socket, FIONBIO, (unsigned long *)&ul); }

public: // socket interface
    inline SOCKET create(int af, int type, int protocol) { m_socket = ::socket(af, type, protocol); return m_socket; }
    inline int getsockopt( int level, int optname, char *optval, int *optlen) { return ::getsockopt(m_socket, level, optname, optval, optlen); }
    inline int connect(const struct sockaddr *name, int namelen) { return ::connect(m_socket, name, namelen); }
    inline int bind(const struct sockaddr *name, int namelen) { return ::bind(m_socket, name, namelen); }
    inline int listen(int backlog) { return ::listen(m_socket, backlog); }
    inline SOCKET accept(struct sockaddr *addr, int *addrlen) { return ::accept(m_socket, addr, addrlen); }
    inline int send(const char *buf, int len, int flags) { return ::send(m_socket, buf, len, flags); }
    inline int recv(char *buf, int len, int flags) { return ::recv(m_socket, buf, len, flags); }
    inline void shutdown() { ::shutdown(m_socket, SD_BOTH); }
    inline void close() { closesocket(m_socket); m_socket = INVALID_SOCKET; }

private:
    SOCKET m_socket;
};

#endif // MODBUSTCP_WIN_H

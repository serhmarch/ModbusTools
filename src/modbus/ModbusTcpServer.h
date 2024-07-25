/*!
 * \file   ModbusTcpServer.h
 * \brief  Header file of Modbus TCP server.
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef MODBUSSERVERTCP_H
#define MODBUSSERVERTCP_H

#include "ModbusServerPort.h"

class ModbusTcpSocket;

/*! \brief The `ModbusTcpServer` class implements TCP server part of the Modbus protocol.

    \details `ModbusTcpServer` ...

 */

class MODBUS_EXPORT ModbusTcpServer : public ModbusServerPort
{
public:
    /*! \brief `Defaults` class constain default settings values for `ModbusTcpServer`.
     */
    struct MODBUS_EXPORT Defaults
    {
        const uint16_t port   ; ///< Default setting 'TCP port number' for the listening server
        const uint32_t timeout; ///< Default setting for the read timeout of every single conncetion

        ///  \details Constructor of the class.
        Defaults();

        /// \details Returns a reference to the global default value object.
        static const Defaults &instance();
    };

public:
    ///  \details Constructor of the class. `device` param is object which might process incoming requests for read/write memory.
    ModbusTcpServer(ModbusInterface *device);

public:
    ///  \details Returns the setting for the TCP port number of the server.
    uint16_t port() const;

    ///  \details Sets the settings for the TCP port number of the server.
    void setPort(uint16_t port);

    ///  \details Returns the setting for the read timeout of every single conncetion.
    int timeout() const;

    ///  \details Sets the setting for the read timeout of every single conncetion.
    void setTimeout(int timeout);

public:
    /// \details Returns the Modbus protocol type. In this case it is `Modbus::TCP`.
    Modbus::ProtocolType type() const override { return Modbus::TCP; }

    /// \details Returns `true`.
    bool isTcpServer() const override { return true; }

    /// \details Try to listen for incoming connections on TCP port that was previously set (`port()`).
    /// \returns \li `Modbus::Status_Good` on success
    ///          \li `Modbus::Status_Processing` when operation is not complete
    ///          \li `Modbus::Status_BadTcpCreate` when can't create TCP socket
    ///          \li `Modbus::Status_BadTcpBind` when can't bind TCP socket
    ///          \li `Modbus::Status_BadTcpListen` when can't listen TCP socket
    Modbus::StatusCode open() override;

    /// \details Stop listening for incoming connections and close all previously opened connections.
    /// \returns \li `Modbus::Status_Good` on success
    ///          \li `Modbus::Status_Processing` when operation is not complete
    Modbus::StatusCode close() override;

    /// \details Returns `true` if the server is currently listening for incoming connections, `false` otherwise.
    bool isOpen() const override;

    /// \details Main function of TCP server. Must be called in cycle to perform all incoming TCP connections.
    Modbus::StatusCode process() override;
    
public:
    /// \details Creates `ModbusServerPort` for new incoming connection defined by `ModbusTcpSocket` pointer/
    virtual ModbusServerPort *createTcpPort(ModbusTcpSocket *socket);
    
public: // SIGNALS
    /// \details Signal occured when new TCP connection was accepted. `source` - name of the current connection.
    void signalNewConnection(const Modbus::Char *source);

    /// \details Signal occured when TCP connection was closed. `source` - name of the current connection.
    void signalCloseConnection(const Modbus::Char *source);

protected:
    /// \details Checks for incoming connections and returns pointer `ModbusTcpSocket` if new connection established, `nullptr` otherwise.
    ModbusTcpSocket *nextPendingConnection();

    /// \details Clear all allocated memory for previously established connections.
    void clearConnections();

protected:
    using ModbusServerPort::ModbusServerPort;
};

#endif // MODBUSSERVERTCP_H

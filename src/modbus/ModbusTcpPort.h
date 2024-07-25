/*!
 * \file   ModbusTcpPort.h
 * \brief  Header file of class `ModbusTcpPort`.
 *
 * \author march
 * \date   April 2024
 */
#ifndef MODBUSTCPPORT_H
#define MODBUSTCPPORT_H

#include "ModbusPort.h"

class ModbusTcpSocket;

/*! \brief Class `ModbusTcpPort` implements TCP version of Modbus protocol.

    \details `ModbusPort` contains function to work with TCP-port (connection).

 */

class MODBUS_EXPORT ModbusTcpPort : public ModbusPort
{
public:
    /*! \brief `Defaults` class constain default settings values for `ModbusTcpPort`.
     */
    struct MODBUS_EXPORT Defaults
    {
        const Modbus::Char *host   ; ///< Default setting 'TCP host name (DNS or IP address)'
        const uint16_t      port   ; ///< Default setting 'TCP port number' for the listening server
        const uint32_t      timeout; ///< Default setting for the read timeout of every single conncetion

        ///  \details Constructor of the class.
        Defaults();

        /// \details Returns a reference to the global default value object.
        static const Defaults &instance();
    };

public:
    /// \details Constructor of the class.
    ModbusTcpPort(ModbusTcpSocket *socket, bool blocking = false);

    /// \details Constructor of the class.
    ModbusTcpPort(bool blocking = false);

public:
    /// \details Returns the Modbus protocol type. In this case it is `Modbus::TCP`.
    Modbus::ProtocolType type() const override { return Modbus::TCP; }

    /// \details Native OS handle for the socket.
    Modbus::Handle handle() const override;

    Modbus::StatusCode open() override;
    Modbus::StatusCode close() override;
    bool isOpen() const override;

public:
    ///  \details Returns the settings for the IP address or DNS name of the remote device.
    const Modbus::Char *host() const;

    ///  \details Sets the settings for the IP address or DNS name of the remote device.
    void setHost(const Modbus::Char *host);

    ///  \details Returns the setting for the TCP port number of the remote device.
    uint16_t port() const;

    ///  \details Sets the settings for the TCP port number of the remote device.
    void setPort(uint16_t port);

    ///  \details Returns the setting for the connection timeout of the remote device.
    uint32_t timeout() const;

    ///  \details Sets the setting for the connection timeout of the remote device.
    void setTimeout(uint32_t timeout);

    void setNextRequestRepeated(bool v) override;

    /// \details Returns `true' if the identifier of each subsequent parcel is automatically incremented by 1, `false' otherwise.
    bool autoIncrement() const;

public:
    const uint8_t *readBufferData() const override;
    uint16_t readBufferSize() const override;
    const uint8_t *writeBufferData() const override;
    uint16_t writeBufferSize() const override;

protected:
    Modbus::StatusCode write() override;
    Modbus::StatusCode read() override;
    Modbus::StatusCode writeBuffer(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff) override;
    Modbus::StatusCode readBuffer(uint8_t &unit, uint8_t &func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff) override;

protected:
    using ModbusPort::ModbusPort;
};

#endif // MODBUSTCPPORT_H

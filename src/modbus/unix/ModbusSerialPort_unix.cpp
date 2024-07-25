#include "../ModbusSerialPort.h"
#include "ModbusSerialPort_p_unix.h"

#include <termios.h>

ModbusSerialPort::Defaults::Defaults() :
    portName(StringLiteral("/dev/ttyS0")),
    baudRate(9600),
    dataBits(8),
    parity(NoParity),
    stopBits(OneStop),
    flowControl(NoFlowControl),
    timeoutFirstByte(1000),
    timeoutInterByte(50)
{
}

ModbusSerialPortPrivate *ModbusSerialPortPrivate::create(bool blocking)
{
    return new ModbusSerialPortPrivateUnix(blocking);
}

Handle ModbusSerialPort::handle() const
{
    return reinterpret_cast<Handle>(d_unix(d_ptr)->serialPort);
}

StatusCode ModbusSerialPort::open()
{
    ModbusSerialPortPrivateUnix *d = d_unix(d_ptr);
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (d->state)
        {
        case STATE_UNKNOWN:
        case STATE_CLOSED:
        case STATE_WAIT_FOR_OPEN:
        {
            if (isOpen())
            {
                d->state = STATE_BEGIN;
                return Status_Good;
            }

            d->clearChanged();
            struct termios options;
            speed_t sp;
            int flags = O_RDWR | O_NOCTTY;
            if (isBlocking())
            {
                flags |= O_SYNC;
            }
            else
            {
                flags |= O_NONBLOCK;
            }
            d->serialPort = ::open(d->settings.portName.c_str(),  flags);

            if (d->serialPortIsInvalid())
            {
                return d->setError(Status_BadSerialOpen, StringLiteral("Failed to open serial port. Error code: ") + toModbusString(errno));
            }

            fcntl(d->serialPort, F_SETFL, 0);
            /* Configuring serial port */
            tcgetattr(d->serialPort, &options);
            switch(d->settings.baudRate)
            {
            case 1200:  sp = B1200;  break;
            case 2400:  sp = B2400;  break;
            case 4800:  sp = B4800;  break;
            case 9600:  sp = B9600;  break;
            case 19200: sp = B19200; break;
            case 38400: sp = B38400; break;
            case 57600: sp = B57600; break;
            case 115200:sp = B115200;break;
            default:    sp = B9600;  break;
            }

            cfsetispeed(&options, sp);
            cfsetospeed(&options, sp);

            options.c_cflag |= (CLOCAL | CREAD);
            /*data bits*/
            options.c_cflag &= ~CSIZE;
            switch (d->settings.dataBits)
            {
            case 5: options.c_cflag |= CS5; break;
            case 6: options.c_cflag |= CS6; break;
            case 7: options.c_cflag |= CS7; break;
            case 8: options.c_cflag |= CS8; break;
            }
            /*parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~PARODD;
            switch (d->settings.parity)
            {
            case EvenParity: options.c_cflag |= PARENB; break;
            case OddParity:  options.c_cflag |= PARENB; options.c_cflag |= PARODD; break;
            default: break; // TODO: Space, Mark Parities
            }
            /*stop bit*/
            switch (d->settings.stopBits)
            {
            case OneStop:
                options.c_cflag &= ~CSTOPB;  // Clear CSTOPB flag for 1 stop bit
                break;
            case OneAndHalfStop:
                options.c_cflag |= CSTOPB;   // Set CSTOPB flag for 2 stop bits (1.5 stop bits not directly supported, use 2)
                break;
            case TwoStop:
                options.c_cflag |= CSTOPB;   // Set CSTOPB flag for 2 stop bits
                break;
            }
            /*disable hardware flow control*/
            /*options.c_cflag &= ~CNEW_RTSCTS;*/
            /*setting Raw input mode*/
            options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
            /*ignore parity errors (we are using CRC!)*/
            options.c_iflag &= ~INPCK;
            options.c_iflag |= IGNPAR;
            /*disable software flow control*/
            /*options.c_iflag &= ~(PARMRK | ISTRIP | IXON | IXOFF | IXANY | ICRNL | IGNBRK | BRKINT | INLCR | IGNCR | ICRNL | IUCLC | IMAXBEL);*/
            options.c_iflag = 0;
            /*setting Raw output mode*/
            /*options.c_oflag &= ~OPOST;*/
            options.c_oflag = 0;
            /*setting serial read timeouts*/
            if (isBlocking())
            {
                options.c_cc[VMIN]  = 0;
                options.c_cc[VTIME] = d->settings.timeoutFirstByte / 100;
            }
            else
            {
                options.c_cc[VMIN]  = 0;
                options.c_cc[VTIME] = 0;
            }
            tcsetattr(d->serialPort, TCSANOW, &options);
        }
            return Status_Good;
        default:
            if (!isOpen())
            {
                d->state = STATE_CLOSED;
                fRepeatAgain = true;
                break;
            }
            return Status_Good;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

StatusCode ModbusSerialPort::close()
{
    ModbusSerialPortPrivateUnix *d = d_unix(d_ptr);
    if (d->serialPortIsOpen())
    {
        d->serialPortClose();
        //setMessage(QString("Serial port '%1' is closed").arg(serialPortName()));
    }
    d->state = STATE_CLOSED;
    return Status_Good;
}

bool ModbusSerialPort::isOpen() const
{
    return d_unix(d_ptr)->serialPortIsOpen();
}

StatusCode ModbusSerialPort::write()
{
    ModbusSerialPortPrivateUnix *d = d_unix(d_ptr);
    int c;
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (d->state)
        {
        case STATE_BEGIN:
        case STATE_PREPARE_TO_WRITE:
            d->timestampRefresh();
            d->state = STATE_WAIT_FOR_WRITE;
            // no need break
        case STATE_WAIT_FOR_WRITE:
        case STATE_WAIT_FOR_WRITE_ALL:
            // Note: clean read buffer from garbage before write
            tcflush(d->serialPort, TCIFLUSH);
            c = ::write(d->serialPort, d->buff, d->sz);
            if (c >= 0)
            {
                d->state = STATE_BEGIN;
                return Status_Good;
            }
            else
            {
                if (errno != EWOULDBLOCK)
                {
                    d->state = STATE_BEGIN;
                    return d->setError(Status_BadSerialWrite, StringLiteral("Error while writing serial port"));
                }
            }
            break;
        default:
            if (isOpen())
            {
                d->state = STATE_BEGIN;
                fRepeatAgain = true;
            }
            break;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

StatusCode ModbusSerialPort::read()
{
    ModbusSerialPortPrivateUnix *d = d_unix(d_ptr);
    int c;
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch(d->state)
        {
        case STATE_BEGIN:
        case STATE_PREPARE_TO_READ:
            d->timestampRefresh();
            d->state = STATE_WAIT_FOR_READ;
            d->sz = 0;
            // no need break
        case STATE_WAIT_FOR_READ:
            // read first byte state
            c = ::read(d->serialPort, d->buff, d->c_buffSz);
            if (c < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    d->state = STATE_BEGIN;
                    return d->setError(Status_BadSerialRead, StringLiteral("Error while reading serial port "));
                }
            }
            if (c > 0)
            {
                d->sz += static_cast<uint16_t>(c);
                if (d->sz > d->c_buffSz)
                {
                    d->state = STATE_BEGIN;
                    return d->setError(Status_BadReadBufferOverflow, StringLiteral("Serial port's '%1' read-buffer overflow"));
                }
                if (isBlocking())
                {
                    d->state = STATE_BEGIN;
                    return Status_Good;
                }
            }
            else if (timer() - d->timestamp >= timeoutFirstByte()) // waiting timeout read first byte elapsed
            {
                d->state = STATE_BEGIN;
                return d->setError(Status_BadSerialRead, StringLiteral("Error while reading serial port "));
            }
            else
            {
                break;
            }
            d->timestampRefresh();
            d->state = STATE_WAIT_FOR_READ_ALL;
            // no need break
        case STATE_WAIT_FOR_READ_ALL:
            // next bytes state
            c = ::read(d->serialPort, d->buff, d->c_buffSz);
            if (c < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    d->state = STATE_BEGIN;
                    return d->setError(Status_BadSerialRead, StringLiteral("Error while reading serial port "));
                }
            }

            if (c > 0)
            {
                d->sz += static_cast<uint16_t>(c);
                if (d->sz > d->c_buffSz)
                    return d->setError(Modbus::Status_BadReadBufferOverflow, StringLiteral("Serial port's read-buffer overflow"));
                d->timestampRefresh();
            }
            else if (timer() - d->timestamp >= timeoutInterByte()) // waiting timeout read next byte elapsed
            {
                d->state = STATE_BEGIN;
                return Status_Good;
            }
            return Status_Processing;
        default:
            if (isOpen())
            {
                d->state = STATE_BEGIN;
                fRepeatAgain = true;
            }
            break;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

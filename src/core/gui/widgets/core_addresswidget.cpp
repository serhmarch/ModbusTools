#include "core_addresswidget.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QSpinBox>

#include <mbcore.h>

mbCoreAddressWidget::mbCoreAddressWidget(QWidget *parent) : QWidget(parent)
{
    m_cmbAdrType = new QComboBox(this);
    m_spAddress  = new SpinBox(this);
    m_notation = mb::Address_Default;

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_cmbAdrType->sizePolicy().hasHeightForWidth());
    m_cmbAdrType->setSizePolicy(sizePolicy);
    m_cmbAdrType->setMinimumWidth(68);
    m_cmbAdrType->setMaximumSize(QSize(100, 16777215));

    m_cmbAdrType->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_0x, m_notation));
    m_cmbAdrType->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_1x, m_notation));
    m_cmbAdrType->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_3x, m_notation));
    m_cmbAdrType->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_4x, m_notation));

    m_spAddress->setMinimumWidth(70);
    setAddressNotation(m_notation);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(1);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_cmbAdrType);
    layout->addWidget(m_spAddress);
    layout->setStretch(0, 0);
    layout->setStretch(1, 1);
}

bool mbCoreAddressWidget::isEnabledAddressType() const
{
    return m_cmbAdrType->isEnabled();
}

void mbCoreAddressWidget::setEnabledAddressType(bool enable)
{
    m_cmbAdrType->setEnabled(enable);
}

bool mbCoreAddressWidget::isEnabledAddress() const
{
    return m_spAddress->isEnabled();
}

void mbCoreAddressWidget::setEnabledAddress(bool enable)
{
    m_spAddress->setEnabled(enable);
}

Modbus::MemoryType mbCoreAddressWidget::addressType() const
{
    switch (m_cmbAdrType->currentIndex())
    {
    case 0: return Modbus::Memory_0x;
    case 1: return Modbus::Memory_1x;
    case 2: return Modbus::Memory_3x;
    case 3: return Modbus::Memory_4x;
    default:
        return Modbus::Memory_4x;
    }

}

mb::Address mbCoreAddressWidget::getAddress() const
{
    mb::Address adr;
    adr.type = addressType();
    switch (m_notation)
    {
    case mb::Address_IEC61131:
    case mb::Address_IEC61131Hex:
        adr.offset = static_cast<quint16>(m_spAddress->value());
        break;
    default:
        adr.offset = static_cast<quint16>(m_spAddress->value()-1);
        break;
    }
    return adr;
}

void mbCoreAddressWidget::setAddress(const mb::Address &adr)
{
    switch (adr.type)
    {
    case Modbus::Memory_0x:
        m_cmbAdrType->setCurrentIndex(0);
        break;
    case Modbus::Memory_1x:
        m_cmbAdrType->setCurrentIndex(1);
        break;
    case Modbus::Memory_3x:
        m_cmbAdrType->setCurrentIndex(2);
        break;
    default:
        m_cmbAdrType->setCurrentIndex(3);
        break;
    }

    switch (m_notation)
    {
    case mb::Address_IEC61131:
    case mb::Address_IEC61131Hex:
        m_spAddress->setValue(adr.offset);
        break;
    default:
        m_spAddress->setValue(adr.offset+1);
        break;
    }
}

void mbCoreAddressWidget::setAddressNotation(int notation)
{
    m_notation = static_cast<mb::AddressNotation>(notation);
    m_cmbAdrType->setItemText(0, mb::toModbusMemoryTypeString(Modbus::Memory_0x, m_notation));
    m_cmbAdrType->setItemText(1, mb::toModbusMemoryTypeString(Modbus::Memory_1x, m_notation));
    m_cmbAdrType->setItemText(2, mb::toModbusMemoryTypeString(Modbus::Memory_3x, m_notation));
    m_cmbAdrType->setItemText(3, mb::toModbusMemoryTypeString(Modbus::Memory_4x, m_notation));
    switch (notation)
    {
    case mb::Address_IEC61131:
        m_spAddress->setMinimum(0);
        m_spAddress->setMaximum(USHRT_MAX);
        m_spAddress->setHex(false);
        break;
    case mb::Address_IEC61131Hex:
        m_spAddress->setMinimum(0);
        m_spAddress->setMaximum(USHRT_MAX);
        m_spAddress->setHex(true);
        break;
    case mb::Address_Modbus:
    case mb::Address_Default:
    default:
        m_spAddress->setMinimum(1);
        m_spAddress->setMaximum(USHRT_MAX+1);
        m_spAddress->setHex(false);
        break;
    }
}

void mbCoreAddressWidget::setAddressType(int type)
{
    switch (type)
    {
    case Modbus::Memory_0x:
        m_cmbAdrType->setCurrentIndex(0);
        break;
    case Modbus::Memory_1x:
        m_cmbAdrType->setCurrentIndex(1);
        break;
    case Modbus::Memory_3x:
        m_cmbAdrType->setCurrentIndex(2);
        break;
    case Modbus::Memory_4x:
    default:
        m_cmbAdrType->setCurrentIndex(3);
        break;
    }
}

mbCoreAddressWidget::SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent)
{
    m_hex = false;
}

QString mbCoreAddressWidget::SpinBox::textFromValue(int value) const
{
    if (m_hex)
        return QString("%1").arg(value, 4, 16, QChar('0')).toUpper();
    return QSpinBox::textFromValue(value);
}

int mbCoreAddressWidget::SpinBox::valueFromText(const QString &text) const
{
    if (m_hex)
    {
        if (text.back() == mb::Strings::instance().IEC61131SuffixHex)
            return text.midRef(0, text.length()-1).toInt(nullptr, 16);
        else
            return text.toInt(nullptr, 16);
    }
    return QSpinBox::valueFromText(text);
}

void mbCoreAddressWidget::SpinBox::setHex(bool hex)
{
    if (m_hex != hex)
    {
        m_hex = hex;
        if (hex)
            setSuffix(mb::Strings::instance().IEC61131SuffixHex);
        else
            setSuffix(QString());
        this->update();
    }
}

QValidator::State mbCoreAddressWidget::SpinBox::validate(QString &input, int &pos) const
{
    if (m_hex)
    {
        QString text = input;
        if (text.back() == mb::Strings::instance().IEC61131SuffixHex)
            text.remove(text.length()-1, 1);
        bool ok;
        text.toInt(&ok, 16); // Validate as hex
        return ok ? QValidator::Acceptable : QValidator::Invalid;
    }
    return QSpinBox::validate(input, pos);
}

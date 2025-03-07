#ifndef CORE_ADDRESSWIDGET_H
#define CORE_ADDRESSWIDGET_H

#include <QSpinBox>
#include <mbcore.h>

class QComboBox;

class MB_EXPORT mbCoreAddressWidget : public QWidget
{
public:
    mbCoreAddressWidget(QWidget *parent = nullptr);

public:
    bool isEnabledAddressType() const;
    void setEnabledAddressType(bool enable);
    bool isEnabledAddress() const;
    void setEnabledAddress(bool enable);
    mb::AddressNotation addressNotation() const { return m_notation; }
    Modbus::MemoryType addressType() const;
    mb::Address getAddress() const;
    void setAddress(const mb::Address &adr);

public Q_SLOTS:
    void setAddressNotation(int notation);
    void setAddressType(int type);

private:
    class SpinBox : public QSpinBox
    {
    public:
        explicit SpinBox(QWidget *parent = nullptr);
        QString textFromValue(int value) const override;
        int valueFromText(const QString &text) const override;
        QValidator::State validate(QString &input, int &pos) const override;

    public:
        inline bool isHex() const { return m_hex; }
        void setHex(bool hex);
    private:
        bool m_hex;
    };

private:
    QComboBox *m_cmbAdrType;
    SpinBox   *m_spAddress ;

private:
    mb::AddressNotation m_notation;
};

#endif // MBCOREADDRESSWIDGET_H

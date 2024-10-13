#ifndef CLIENT_SCANNERUI_H
#define CLIENT_SCANNERUI_H

#include <gui/dialogs/core_dialogbase.h>

#include "client_scanner.h"

namespace Ui {
class mbClientScannerUi;
}

class QListWidget;

class mbClientScannerModel;
class mbClientDialogScannerRequest;

class mbClientScannerUi : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogBase::Strings
    {
        const QString prefix        ;
        const QString type          ;
        const QString timeout       ;
        const QString tries         ;
        const QString unitStart     ;
        const QString unitEnd       ;
        const QString request       ;
        const QString host          ;
        const QString port          ;
        const QString serialPortName;
        const QString baudRateList  ;
        const QString dataBitsList  ;
        const QString parityList    ;
        const QString stopBitsList  ;
        const QString wSplitterState;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientScannerUi(QWidget *parent = nullptr);
    ~mbClientScannerUi();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

private Q_SLOTS:
    void slotEditRequest ();
    void slotEditBaudRate();
    void slotEditDataBits();
    void slotEditParity  ();
    void slotEditStopBits();

private Q_SLOTS:
    void slotAdd();
    void slotAddAll();
    void slotClear();
    void slotStart();
    void slotStop();
    void slotClose();

private Q_SLOTS:
    void setType(int type);
    void stateChange(bool run);
    void setStatDevice(const QString &device);
    void setStatFound  (quint32 count);
    void setStatCountTx(quint32 count);
    void setStatCountRx(quint32 count);
    void setStatPercent(quint32 p);

private:
    void closeEvent(QCloseEvent*) override;
    void timerEvent(QTimerEvent*) override;

private:
    QVariantList getValues(const QListWidget *w) const;
    void setValues(QListWidget *w, const QVariantList &v);
    void setRequest(const mbClientScanner::Request_t &req);
    inline void setRequest(const QString &sReq) { setRequest(mbClientScanner::toRequest(sReq)); }
    void refreshElapsedTime();

private:
    Ui::mbClientScannerUi *ui;
    mbClientScanner *m_scanner;
    mbClientScannerModel *m_model;
    mbClientScanner::Request_t m_request;
    mbClientDialogScannerRequest *m_dialogRequest;
    mb::Timestamp_t m_timestampStart;
    int m_timerId;
};

#endif // CLIENT_SCANNERUI_H

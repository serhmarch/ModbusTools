#ifndef CLIENT_SCANNERUI_H
#define CLIENT_SCANNERUI_H

#include <QDialog>

#include <mbcore.h>

#include "client_scanner.h"

namespace Ui {
class mbClientScannerUi;
}

class QListWidget;

class mbClientScannerModel;
class mbClientDialogScannerRequest;

class mbClientScannerUi : public QDialog
{
    Q_OBJECT

public:
    struct Strings
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
        const QString wGeometry     ;
        const QString wSplitterState;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientScannerUi(QWidget *parent = nullptr);
    ~mbClientScannerUi();

public:
    MBSETTINGS cachedSettings() const;
    void setCachedSettings(const MBSETTINGS &settings);

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

private:
    void closeEvent(QCloseEvent*) override;

private:
    QVariantList getValues(const QListWidget *w) const;
    void setValues(QListWidget *w, const QVariantList &v);
    void setRequest(const mbClientScanner::Request_t &req);
    inline void setRequest(const QString &sReq) { setRequest(mbClientScanner::toRequest(sReq)); }

private:
    Ui::mbClientScannerUi *ui;
    mbClientScanner *m_scanner;
    mbClientScannerModel *m_model;
    mbClientScanner::Request_t m_request;
    mbClientDialogScannerRequest *m_dialogRequest;
};

#endif // CLIENT_SCANNERUI_H

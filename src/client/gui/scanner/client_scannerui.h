#ifndef CLIENT_SCANNERUI_H
#define CLIENT_SCANNERUI_H

#include <QDialog>

namespace Ui {
class mbClientScannerUi;
}

class mbClientScanner;
class mbClientScannerModel;

class mbClientScannerUi : public QDialog
{
    Q_OBJECT

public:
    explicit mbClientScannerUi(QWidget *parent = nullptr);
    ~mbClientScannerUi();

private Q_SLOTS:
    void slotAdd();
    void slotAddAll();
    void slotClear();
    void slotStart();
    void slotStop();
    void slotClose();

private:
    void closeEvent(QCloseEvent*) override;

private:
    Ui::mbClientScannerUi *ui;
    mbClientScanner *m_scanner;
    mbClientScannerModel *m_model;
};

#endif // CLIENT_SCANNERUI_H

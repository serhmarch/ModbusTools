#ifndef CLIENT_DIALOGSCANNERREQUEST_H
#define CLIENT_DIALOGSCANNERREQUEST_H

#include <QDialog>
#include <QAbstractListModel>

#include <mbcore.h>

#include "client_scanner.h"

namespace Ui {
class mbClientDialogScannerRequest;
}

class mbClientDialogScannerRequest : public QDialog
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString prefix        ;
        const QString func          ;
        const QString subfunc       ;
        const QString offset1       ;
        const QString offset2       ;
        const QString count2        ;
        const QString fileRecords   ;
        const QString wGeometry     ;
        const QString wSplitterState;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientDialogScannerRequest(QWidget *parent = nullptr);
    ~mbClientDialogScannerRequest();

public:
    bool getRequest(mbClientScanner::Request_t &req);

public:
    MBSETTINGS cachedSettings() const;
    void setCachedSettings(const MBSETTINGS &settings);

private Q_SLOTS:
    void setFunction(const QModelIndex &current);
    void addFunc();
    void modifyFunc();
    void deleteFunc();
    void moveUpFunc();
    void moveDownFunc();
    void clearFuncs();
    void setCurrentFuncIndex(int i);

    void addFileRecord();
    void deleteFileRecord();
    void moveUpFileRecord();
    void moveDownFileRecord();
    void clearFileRecords();

private:
    mbClientMessageParams getCurrentFunc() const;
    void setCurrentFunc(const mbClientMessageParams &f);
    uint8_t getCurrentFuncNum() const;
    uint16_t getCurrentDiagnSubfuncNum() const;
    void setCurrentFuncNum(uint8_t funcNum);
    void setCurrentDiagnSubfuncNum(uint16_t subfunc);
    QVector<Modbus::FileRecord> getCurrentFileRecords() const;
    void setCurrentFileRecords(const QVector<Modbus::FileRecord> &fileRecords);

private:
    class Model : public QAbstractListModel
    {
    public:
        using QAbstractListModel::QAbstractListModel;

    public: // QAbstractModel
        int rowCount(const QModelIndex &) const override;
        QVariant data(const QModelIndex &index, int role) const override;

    public:
        inline mbClientScanner::Request_t request() const { return m_req; }
        void setRequest(const mbClientScanner::Request_t &req);

    public:
        mbClientMessageParams func(int i);
        void addFunc(const mbClientMessageParams &f);
        void modifyFunc(int i, const mbClientMessageParams &f);
        void deleteFunc(int i);
        void moveUpFunc(int i);
        void moveDownFunc(int i);
        void clearFuncs();

    private:
        mbClientScanner::Request_t m_req;
    };

private:
    Ui::mbClientDialogScannerRequest *ui;
    Model *m_model;
    QList<uint8_t> m_funcNums;
    QList<uint16_t> m_diagnSubfuncNums;
};

#endif // CLIENT_DIALOGSCANNERREQUEST_H

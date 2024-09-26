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
        const QString offset1       ;
        const QString offset2       ;
        const QString count2        ;
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
    void selectionChanged(const QModelIndex &current, const QModelIndex &);
    void addFunc();
    void modifyFunc();
    void deleteFunc();
    void setCurrentFuncIndex(int i);

private:
    mbClientScanner::FuncParams getCurrentFunc() const;
    void setCurrentFunc(const mbClientScanner::FuncParams &f);
    uint8_t getCurrentFuncNum() const;
    void setCurrentFuncNum(uint8_t funcNum);

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
        mbClientScanner::FuncParams func(int i);
        void addFunc(const mbClientScanner::FuncParams &f);
        void modifyFunc(int i, const mbClientScanner::FuncParams &f);
        void deleteFunc(int i);

    private:
        mbClientScanner::Request_t m_req;
    };

private:
    Ui::mbClientDialogScannerRequest *ui;
    Model *m_model;
    QList<uint8_t> m_funcNums;
};

#endif // CLIENT_DIALOGSCANNERREQUEST_H

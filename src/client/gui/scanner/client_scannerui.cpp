#include "client_scannerui.h"
#include "ui_client_scannerui.h"

#include <QHeaderView>
#include <QListWidget>
#include <QListWidgetItem>

#include <client.h>
#include <gui/client_ui.h>
#include <gui/dialogs/client_dialogs.h>

#include "client_scanner.h"
#include "client_scannermodel.h"


mbClientScannerUi::mbClientScannerUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mbClientScannerUi)
{
    ui->setupUi(this);

    const mbClientScanner::Defaults &d = mbClientScanner::Defaults::instance();
    const Modbus::Defaults &md = Modbus::Defaults::instance();

    m_scanner = new mbClientScanner(this);

    QLineEdit *ln;
    QSpinBox* sp;
    QComboBox* cmb;

    // Type
    cmb = ui->cmbType;
    cmb->addItem(Modbus::toString(Modbus::ASC));
    cmb->addItem(Modbus::toString(Modbus::RTU));
    cmb->addItem(Modbus::toString(Modbus::TCP));
    cmb->setCurrentText(Modbus::toString(Modbus::TCP));
    ui->stackedWidget->setCurrentWidget(ui->pgTcpPort);
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));

    // Timeout
    sp = ui->spTimeout;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.timeout);

    // Tries
    sp = ui->spTries;
    sp->setMinimum(1);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.tries);

    // Start Unit
    sp = ui->spUnitStart;
    sp->setMinimum(0);
    sp->setMaximum(UCHAR_MAX);
    sp->setValue(d.unitStart);

    // End Unit
    sp = ui->spUnitEnd;
    sp->setMinimum(0);
    sp->setMaximum(UCHAR_MAX);
    sp->setValue(d.unitEnd);

    //--------------------- SERIAL ---------------------
    // Serial Port
    cmb = ui->cmbSerialPortName;
    QStringList ports = Modbus::availableSerialPortList();
    Q_FOREACH(const QString &port, ports)
        cmb->addItem(port);

    //--------------------- TCP ---------------------
    // Host
    ln = ui->lnTcpHost;
    ln->setText(md.host);
    // Port
    sp = ui->spTcpPort;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(md.port);

    m_model = new mbClientScannerModel(m_scanner, this);
    ui->tableView->setModel(m_model);
    QHeaderView *header;
    header = ui->tableView->horizontalHeader();
    header->setStretchLastSection(true);
    header->setVisible(false);
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header = ui->tableView->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->btnEditBaudRate, &QPushButton::clicked, this, &mbClientScannerUi::slotEditBaudRate);
    connect(ui->btnEditDataBits, &QPushButton::clicked, this, &mbClientScannerUi::slotEditDataBits);
    connect(ui->btnEditParity  , &QPushButton::clicked, this, &mbClientScannerUi::slotEditParity  );
    connect(ui->btnEditStopBits, &QPushButton::clicked, this, &mbClientScannerUi::slotEditStopBits);

    connect(ui->btnAdd   , &QPushButton::clicked, this, &mbClientScannerUi::slotAdd   );
    connect(ui->btnAddAll, &QPushButton::clicked, this, &mbClientScannerUi::slotAddAll);
    connect(ui->btnClear , &QPushButton::clicked, this, &mbClientScannerUi::slotClear );
    connect(ui->btnStart , &QPushButton::clicked, this, &mbClientScannerUi::slotStart );
    connect(ui->btnStop  , &QPushButton::clicked, this, &mbClientScannerUi::slotStop  );
    connect(ui->btnClose , &QPushButton::clicked, this, &mbClientScannerUi::slotClose );
}

mbClientScannerUi::~mbClientScannerUi()
{
    delete ui;
}

void mbClientScannerUi::slotEditBaudRate()
{
    QVariantList ls = getValues(ui->lsBaudRate);
    bool res = mbClient::global()->ui()->dialogs()->getValueList(mbCore::Defaults::instance().availableBaudRate, ls);
    if (res)
        setValues(ui->lsBaudRate, ls);
}

void mbClientScannerUi::slotEditDataBits()
{
    QVariantList ls = getValues(ui->lsDataBits);
    bool res = mbClient::global()->ui()->dialogs()->getValueList(mbCore::Defaults::instance().availableDataBits, ls);
    if (res)
        setValues(ui->lsDataBits, ls);
}

void mbClientScannerUi::slotEditParity()
{
    QVariantList ls = getValues(ui->lsParity);
    bool res = mbClient::global()->ui()->dialogs()->getValueList(mbCore::Defaults::instance().availableParity, ls);
    if (res)
        setValues(ui->lsParity, ls);
}

void mbClientScannerUi::slotEditStopBits()
{
    QVariantList ls = getValues(ui->lsStopBits);
    bool res = mbClient::global()->ui()->dialogs()->getValueList(mbCore::Defaults::instance().availableStopBits, ls);
    if (res)
        setValues(ui->lsStopBits, ls);
}

void mbClientScannerUi::slotAdd()
{

}

void mbClientScannerUi::slotAddAll()
{

}

void mbClientScannerUi::slotClear()
{
    m_scanner->clear();
}

void mbClientScannerUi::slotStart()
{
    Modbus::Settings s;
    Modbus::ProtocolType type = static_cast<Modbus::ProtocolType>(ui->cmbType->currentIndex());
    mbClientScanner::setSettingUnitStart(s, ui->spUnitStart->value());
    mbClientScanner::setSettingUnitEnd(s, ui->spUnitEnd->value());
    Modbus::setSettingType(s, type);
    Modbus::setSettingTimeout(s, ui->spTimeout->value());
    Modbus::setSettingSerialPortName(s, ui->cmbSerialPortName->currentText());
    mbClientScanner::setSettingBaudRate(s, getValues(ui->lsBaudRate));
    mbClientScanner::setSettingDataBits(s, getValues(ui->lsDataBits));
    mbClientScanner::setSettingParity  (s, getValues(ui->lsParity  ));
    mbClientScanner::setSettingStopBits(s, getValues(ui->lsStopBits));
    Modbus::setSettingFlowControl(s, Modbus::NoFlowControl);
    m_scanner->startScanning(s);
}

void mbClientScannerUi::slotStop()
{
    m_scanner->stopScanning();
}

void mbClientScannerUi::slotClose()
{
    QDialog::close();
}

void mbClientScannerUi::setType(int type)
{
    switch (type)
    {
    case Modbus::TCP:
        ui->stackedWidget->setCurrentWidget(ui->pgTcpPort);
        break;
    case Modbus::ASC:
    case Modbus::RTU:
        ui->stackedWidget->setCurrentWidget(ui->pgSerialPort);
        break;
    }
}

void mbClientScannerUi::closeEvent(QCloseEvent *)
{
    //m_scanner->stopScanning();
}

QVariantList mbClientScannerUi::getValues(const QListWidget *w)
{
    QVariantList ls;
    for (int i = 0; i < w->count(); i++)
        ls.append(w->item(i)->data(Qt::DisplayRole));
    return ls;
}

void mbClientScannerUi::setValues(QListWidget *w, const QVariantList &ls)
{
    w->clear();
    Q_FOREACH(const QVariant &v, ls)
        w->addItem(v.toString());
}

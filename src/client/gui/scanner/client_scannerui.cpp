#include "client_scannerui.h"
#include "ui_client_scannerui.h"

#include <QHeaderView>

#include "client_scanner.h"
#include "client_scannermodel.h"

mbClientScannerUi::mbClientScannerUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mbClientScannerUi)
{
    ui->setupUi(this);

    const mbClientScanner::Defaults &d = mbClientScanner::Defaults::instance();

    m_scanner = new mbClientScanner(this);

    QSpinBox* sp;
    QComboBox* cmb;

    // Type
    cmb = ui->cmbType;
    cmb->addItem(Modbus::toString(Modbus::ASC));
    cmb->addItem(Modbus::toString(Modbus::RTU));
    cmb->addItem(Modbus::toString(Modbus::TCP));
    cmb->setCurrentText(Modbus::toString(Modbus::TCP));

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

    m_model = new mbClientScannerModel(m_scanner, this);
    ui->tableView->setModel(m_model);
    QHeaderView *header;
    header = ui->tableView->horizontalHeader();
    header->setStretchLastSection(true);
    header->setVisible(false);
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header = ui->tableView->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

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

    mbClientScanner::setSettingUnitStart(s, ui->spUnitStart->value());
    mbClientScanner::setSettingUnitEnd(s, ui->spUnitEnd->value());
    Modbus::setSettingType(s, static_cast<Modbus::ProtocolType>(ui->cmbType->currentIndex()));
    Modbus::setSettingTimeout(s, ui->spTimeout->value());
    Modbus::setSettingSerialPortName(s, ui->cmbSerialPortName->currentText());
    Modbus::setSettingBaudRate(s, 9600);
    Modbus::setSettingDataBits(s, 8);
    Modbus::setSettingParity(s, Modbus::NoParity);
    Modbus::setSettingStopBits(s, Modbus::OneStop);
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

void mbClientScannerUi::closeEvent(QCloseEvent *)
{
    //m_scanner->stopScanning();
}

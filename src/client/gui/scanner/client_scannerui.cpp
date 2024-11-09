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
#include "client_dialogscannerrequest.h"

mbClientScannerUi::Strings::Strings() : mbCoreDialogBase::Strings(),
    prefix        (QStringLiteral("Ui.Scanner.")),
    type          (prefix+Modbus::Strings::instance().type),
    timeout       (prefix+Modbus::Strings::instance().timeout),
    tries         (prefix+mbClientScanner::Strings::instance().tries    ),
    unitStart     (prefix+mbClientScanner::Strings::instance().unitStart),
    unitEnd       (prefix+mbClientScanner::Strings::instance().unitEnd  ),
    request       (prefix+mbClientScanner::Strings::instance().request  ),
    host          (prefix+Modbus::Strings::instance().host),
    port          (prefix+Modbus::Strings::instance().port),
    serialPortName(prefix+Modbus::Strings::instance().serialPortName),
    baudRateList  (QStringLiteral("baudRateList")),
    dataBitsList  (QStringLiteral("dataBitsList")),
    parityList    (QStringLiteral("parityList")),
    stopBitsList  (QStringLiteral("stopBitsList")),
    wSplitterState(QStringLiteral("splitterState"))
{

}

const mbClientScannerUi::Strings &mbClientScannerUi::Strings::instance()
{
    static Strings s;
    return s;
}

mbClientScannerUi::mbClientScannerUi(QWidget *parent) :
    mbCoreDialogBase(Strings::instance().prefix, parent),
    ui(new Ui::mbClientScannerUi)
{
    ui->setupUi(this);

    const mbClientScanner::Defaults &d = mbClientScanner::Defaults::instance();
    const Modbus::Defaults &md = Modbus::Defaults::instance();

    m_scanner = new mbClientScanner(this);
    setStatDevice (m_scanner->statDevice ());
    setStatFound  (m_scanner->statFound  ());
    setStatCountTx(m_scanner->statCountTx());
    setStatCountRx(m_scanner->statCountRx());
    setStatPercent(m_scanner->statPercent());
    connect(m_scanner, &mbClientScanner::stateChanged, this, &mbClientScannerUi::stateChange);
    connect(m_scanner, &mbClientScanner::statDeviceChanged , this, &mbClientScannerUi::setStatDevice );
    connect(m_scanner, &mbClientScanner::statFoundChanged  , this, &mbClientScannerUi::setStatFound  );
    connect(m_scanner, &mbClientScanner::statCountTxChanged, this, &mbClientScannerUi::setStatCountTx);
    connect(m_scanner, &mbClientScanner::statCountRxChanged, this, &mbClientScannerUi::setStatCountRx);
    connect(m_scanner, &mbClientScanner::statPercentChanged, this, &mbClientScannerUi::setStatPercent);

    QVariantList vls;
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
    cmb->setEditable(true); // Note: Allow user right to enter port name if it's absent in list

    // Baud Rate List
    vls.clear();
    vls.append(QString::number(md.baudRate));
    setValues(ui->lsBaudRate, vls);

    // Data Bits List
    vls.clear();
    vls.append(QString::number(md.dataBits));
    setValues(ui->lsDataBits, vls);

    // Parity List
    vls.clear();
    vls.append(Modbus::toString(md.parity));
    setValues(ui->lsParity, vls);

    // Stop Bits List
    vls.clear();
    vls.append(Modbus::toString(md.stopBits));
    setValues(ui->lsStopBits, vls);

    //--------------------- TCP ---------------------
    // Host
    ln = ui->lnTcpHost;
    ln->setText(md.host);
    // Port
    sp = ui->spTcpPort;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(md.port);

    //-------------------- OTHER --------------------
    setRequest(d.request);

    m_model = new mbClientScannerModel(m_scanner, this);
    ui->tableView->setModel(m_model);
    QHeaderView *header;
    header = ui->tableView->horizontalHeader();
    header->setStretchLastSection(true);
    header->setVisible(false);
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header = ui->tableView->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_dialogRequest = new mbClientDialogScannerRequest(this);

    connect(ui->btnEditRequest , &QPushButton::clicked, this, &mbClientScannerUi::slotEditRequest );
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

    m_timerId = -1;
}

mbClientScannerUi::~mbClientScannerUi()
{
    delete ui;
}

MBSETTINGS mbClientScannerUi::cachedSettings() const
{
    MBSETTINGS m = mbCoreDialogBase::cachedSettings();
    mb::unite(m, m_dialogRequest->cachedSettings());

    const Strings &s = Strings::instance();

    m[s.type          ] = ui->cmbType          ->currentText();
    m[s.timeout       ] = ui->spTimeout        ->value      ();
    m[s.tries         ] = ui->spTries          ->value      ();
    m[s.unitStart     ] = ui->spUnitStart      ->value      ();
    m[s.unitEnd       ] = ui->spUnitEnd        ->value      ();
    m[s.request       ] = mbClientScanner::toString(m_request);
    m[s.host          ] = ui->lnTcpHost        ->text       ();
    m[s.port          ] = ui->spTcpPort        ->value      ();
    m[s.serialPortName] = ui->cmbSerialPortName->currentText();
    m[s.baudRateList  ] = getValues(ui->lsBaudRate);
    m[s.dataBitsList  ] = getValues(ui->lsDataBits);
    m[s.parityList    ] = getValues(ui->lsParity  );
    m[s.stopBitsList  ] = getValues(ui->lsStopBits);
    m[s.wSplitterState] = ui->splitter->saveState();

    return m;
}

void mbClientScannerUi::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogBase::setCachedSettings(m);
    m_dialogRequest->setCachedSettings(m);

    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(s.type          ); if (it != end) ui->cmbType          ->setCurrentText(it.value().toString());
    it = m.find(s.timeout       ); if (it != end) ui->spTimeout        ->setValue      (it.value().toInt()   );
    it = m.find(s.tries         ); if (it != end) ui->spTries          ->setValue      (it.value().toInt()   );
    it = m.find(s.unitStart     ); if (it != end) ui->spUnitStart      ->setValue      (it.value().toInt()   );
    it = m.find(s.unitEnd       ); if (it != end) ui->spUnitEnd        ->setValue      (it.value().toInt()   );
    it = m.find(s.request       ); if (it != end) this                 ->setRequest    (it.value().toString());
    it = m.find(s.host          ); if (it != end) ui->lnTcpHost        ->setText       (it.value().toString());
    it = m.find(s.port          ); if (it != end) ui->spTcpPort        ->setValue      (it.value().toInt()   );
    it = m.find(s.serialPortName); if (it != end) ui->cmbSerialPortName->setCurrentText(it.value().toString());
    it = m.find(s.baudRateList  ); if (it != end) setValues(ui->lsBaudRate, it.value().toList());
    it = m.find(s.dataBitsList  ); if (it != end) setValues(ui->lsDataBits, it.value().toList());
    it = m.find(s.parityList    ); if (it != end) setValues(ui->lsParity  , it.value().toList());
    it = m.find(s.stopBitsList  ); if (it != end) setValues(ui->lsStopBits, it.value().toList());
    it = m.find(s.wSplitterState); if (it != end) ui->splitter         ->restoreState   (it.value().toByteArray());
}

void mbClientScannerUi::slotEditRequest()
{
    mbClientScanner::Request_t req = m_request;
    if (m_dialogRequest->getRequest(req))
        setRequest(req);
}

void mbClientScannerUi::slotEditBaudRate()
{
    QVariantList ls = getValues(ui->lsBaudRate);
    bool res = mbClient::global()->ui()->dialogs()->getValueList(mbCore::Defaults::instance().availableBaudRate, ls, QStringLiteral("Edit Baud Rate"));
    if (res)
        setValues(ui->lsBaudRate, ls);
}

void mbClientScannerUi::slotEditDataBits()
{
    QVariantList ls = getValues(ui->lsDataBits);
    bool res = mbClient::global()->ui()->dialogs()->getValueList(mbCore::Defaults::instance().availableDataBits, ls, QStringLiteral("Edit Data Bits"));
    if (res)
        setValues(ui->lsDataBits, ls);
}

void mbClientScannerUi::slotEditParity()
{
    QVariantList ls = getValues(ui->lsParity);
    bool res = mbClient::global()->ui()->dialogs()->getValueList(mbCore::Defaults::instance().availableParity, ls, QStringLiteral("Edit Parity"));
    if (res)
        setValues(ui->lsParity, ls);
}

void mbClientScannerUi::slotEditStopBits()
{
    QVariantList ls = getValues(ui->lsStopBits);
    bool res = mbClient::global()->ui()->dialogs()->getValueList(mbCore::Defaults::instance().availableStopBits, ls, QStringLiteral("Edit Stop Bits"));
    if (res)
        setValues(ui->lsStopBits, ls);
}

void mbClientScannerUi::slotAdd()
{
    QList<int> indexes;
    QModelIndexList rows = ui->tableView->selectionModel()->selectedRows();
    Q_FOREACH(const QModelIndex &row, rows)
        indexes.append(row.row());
    if (indexes.count())
        m_scanner->addToProject(indexes);
}

void mbClientScannerUi::slotAddAll()
{
    m_scanner->addToProject();
}

void mbClientScannerUi::slotClear()
{
    m_scanner->clear();
}

void mbClientScannerUi::slotStart()
{
    m_scanner->clear();

    Modbus::Settings s;
    Modbus::ProtocolType type = static_cast<Modbus::ProtocolType>(ui->cmbType->currentIndex());
    mbClientScanner::setSettingUnitStart(s, ui->spUnitStart->value());
    mbClientScanner::setSettingUnitEnd(s, ui->spUnitEnd->value());
    Modbus::setSettingType(s, type);
    Modbus::setSettingTimeout(s, ui->spTimeout->value());
    Modbus::setSettingTries(s, ui->spTries->value());
    Modbus::setSettingHost(s, ui->lnTcpHost->text());
    Modbus::setSettingPort(s, ui->spTcpPort->value());
    Modbus::setSettingSerialPortName(s, ui->cmbSerialPortName->currentText());
    mbClientScanner::setSettingBaudRate(s, getValues(ui->lsBaudRate));
    mbClientScanner::setSettingDataBits(s, getValues(ui->lsDataBits));
    mbClientScanner::setSettingParity  (s, getValues(ui->lsParity  ));
    mbClientScanner::setSettingStopBits(s, getValues(ui->lsStopBits));
    Modbus::setSettingFlowControl(s, Modbus::NoFlowControl);
    mbClientScanner::setSettingRequest(s, m_request);

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

void mbClientScannerUi::stateChange(bool run)
{
    if (run)
    {
        m_timestampStart = mb::currentTimestamp();
        ui->lbTmStart->setText(mb::toString(m_timestampStart));
        m_timerId = this->startTimer(1000);
        refreshElapsedTime();
    }
    else
    {
        killTimer(m_timerId);
        m_timerId = -1;
    }
    bool enable = !run;
    ui->btnAdd   ->setEnabled(enable);
    ui->btnAddAll->setEnabled(enable);
    ui->btnClear ->setEnabled(enable);
    ui->btnStart ->setEnabled(enable);
    ui->grCommon ->setEnabled(enable);
    ui->grPort   ->setEnabled(enable);
}

void mbClientScannerUi::setStatDevice(const QString &device)
{
    ui->lbStatDevice->setText(device);
}

void mbClientScannerUi::setStatFound(quint32 count)
{
    ui->lbStatFound->setText(QString::number(count));
}

void mbClientScannerUi::setStatCountTx(quint32 count)
{
    ui->lbStatTx->setText(QString::number(count));
}

void mbClientScannerUi::setStatCountRx(quint32 count)
{
    ui->lbStatRx->setText(QString::number(count));
}

void mbClientScannerUi::setStatPercent(quint32 p)
{
    ui->progressBar->setValue(p);
}

void mbClientScannerUi::closeEvent(QCloseEvent *)
{
    //m_scanner->stopScanning();
}

void mbClientScannerUi::timerEvent(QTimerEvent *)
{
    refreshElapsedTime();
}

QVariantList mbClientScannerUi::getValues(const QListWidget *w) const
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

void mbClientScannerUi::setRequest(const mbClientScanner::Request_t &req)
{
    m_request = req;
    if (m_request.count() == 0)
        m_request.append(mbClientScanner::Defaults::instance().request);
    QString s;
    if (m_request.count() == 1)
        s = mbClientScanner::toString(m_request.first());
    else
        s = QString("%1 functions").arg(m_request.count());
    ui->lnRequest->setText(s);
}

void mbClientScannerUi::refreshElapsedTime()
{
    qint64 tm = mb::currentTimestamp() - m_timestampStart;
    tm = tm / 1000;  // Note: convert to seconds
    int sec = tm % 60; tm = tm / 60;
    int min = tm % 60; tm = tm / 60;
    int hr = tm;
    QString s = QString("%1:%2:%3")
                .arg(hr)
                .arg(min, 2, 10, QLatin1Char('0'))
                .arg(sec, 2, 10, QLatin1Char('0'));
    ui->lbTmElapsed->setText(s);
}

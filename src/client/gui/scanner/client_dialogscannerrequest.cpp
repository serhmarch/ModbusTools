#include "client_dialogscannerrequest.h"
#include "ui_client_dialogscannerrequest.h"

#include <mbcore.h>

mbClientDialogScannerRequest::Strings::Strings() :
    prefix        (QStringLiteral("Ui.Scanner.Dialogs.Request.")),
    func          (prefix+QStringLiteral("func")),
    subfunc       (prefix+QStringLiteral("subfunc")),
    offset1       (prefix+QStringLiteral("offset1")),
    offset2       (prefix+QStringLiteral("offset2")),
    count2        (prefix+QStringLiteral("count2")),
    fileRecords   (prefix+QStringLiteral("fileRecords")),
    wGeometry     (prefix+QStringLiteral("geometry")),
    wSplitterState(prefix+QStringLiteral("splitterState"))
{
}

const mbClientDialogScannerRequest::Strings &mbClientDialogScannerRequest::Strings::instance()
{
    static Strings s;
    return s;
}

mbClientDialogScannerRequest::mbClientDialogScannerRequest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mbClientDialogScannerRequest)
{
    ui->setupUi(this);

    m_model = new Model(this);

    ui->lsRequest->setModel(m_model);
    ui->lsRequest->setSelectionMode(QAbstractItemView::SingleSelection);

    QSpinBox *sp;
    QComboBox *cmb;

    sp = ui->spOffset1;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(0);

    sp = ui->spOffset2;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(0);

    sp = ui->spCount2;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(1);

    // device id
    sp = ui->spDeviceId;
    sp->setMinimum(0);
    sp->setMaximum(UINT8_MAX);
    sp->setValue(1);

    // object id
    sp = ui->spObjectId;
    sp->setMinimum(0);
    sp->setMaximum(UINT8_MAX);
    sp->setValue(0);

    m_funcNums.append(MBF_READ_COILS                   );
    m_funcNums.append(MBF_READ_DISCRETE_INPUTS         );
    m_funcNums.append(MBF_READ_HOLDING_REGISTERS       );
    m_funcNums.append(MBF_READ_INPUT_REGISTERS         );
    m_funcNums.append(MBF_WRITE_SINGLE_COIL            );
    m_funcNums.append(MBF_WRITE_SINGLE_REGISTER        );
    m_funcNums.append(MBF_READ_EXCEPTION_STATUS        );
    m_funcNums.append(MBF_DIAGNOSTICS                  );
    m_funcNums.append(MBF_GET_COMM_EVENT_COUNTER       );
    m_funcNums.append(MBF_GET_COMM_EVENT_LOG           );
    m_funcNums.append(MBF_WRITE_MULTIPLE_COILS         );
    m_funcNums.append(MBF_WRITE_MULTIPLE_REGISTERS     );
    m_funcNums.append(MBF_REPORT_SERVER_ID             );
    m_funcNums.append(MBF_READ_FILE_RECORD             );
    m_funcNums.append(MBF_WRITE_FILE_RECORD            );
    m_funcNums.append(MBF_MASK_WRITE_REGISTER          );
    m_funcNums.append(MBF_READ_WRITE_MULTIPLE_REGISTERS);
    m_funcNums.append(MBF_READ_FIFO_QUEUE              );

    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_QUERY_DATA                     );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RESTART_COMMUNICATIONS_OPTION         );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_DIAGNOSTIC_REGISTER            );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_CHANGE_ASCII_INPUT_DELIMITER          );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_FORCE_LISTEN_ONLY_MODE                );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_CLEAR_COUNTERS_AND_DIAGNOSTIC_REGISTER);
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_BUS_MESSAGE_COUNT              );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_BUS_COMMUNICATION_ERROR_COUNT  );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_BUS_EXCEPTION_ERROR_COUNT      );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_SERVER_MESSAGE_COUNT           );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_SERVER_NO_RESPONSE_COUNT       );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_SERVER_NAK_COUNT               );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_SERVER_BUSY_COUNT              );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_RETURN_BUS_CHARACTER_OVERRUN_COUNT    );
    m_diagnSubfuncNums.append(MBF_DIAGNOSTICS_CLEAR_OVERRUN_COUNTER_AND_FLAG        );

    cmb = ui->cmbFunction;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentFuncIndex(int)));
    Q_FOREACH (uint8_t funcNum, m_funcNums)
    {
        cmb->addItem(QString("%1 - %2")
                    .arg(funcNum, 2, 10, QChar('0'))
                    .arg(mb::ModbusFunctionString(funcNum))
                    );
    }
    m_funcNums.append(MBF_ENCAPSULATED_INTERFACE_TRANSPORT);
    cmb->addItem(QString("%1/%2 - ReadDeviceIdentification")
                     .arg(MBF_ENCAPSULATED_INTERFACE_TRANSPORT, 2, 10, QChar('0'))
                     .arg(MBF_MEI_READ_DEVICE_ID, 2, 10, QChar('0')));
    cmb->setCurrentIndex(2);

    cmb = ui->cmbDiagnSubfunction;
    //connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentDiagnSubfuncNum(int)));
    Q_FOREACH (auto funcNum, m_diagnSubfuncNums)
    {
        cmb->addItem(QString("%1 - %2")
                         .arg(funcNum, 2, 10, QChar('0'))
                         .arg(mb::ModbusDiagnSubfunctionString(funcNum))
                     );
    }
    cmb->setCurrentIndex(0);

    connect(ui->lsRequest, &QListView::doubleClicked, this, &mbClientDialogScannerRequest::setFunction);
    connect(ui->btnAdd   , &QPushButton::clicked, this, &mbClientDialogScannerRequest::addFunc   );
    connect(ui->btnModify, &QPushButton::clicked, this, &mbClientDialogScannerRequest::modifyFunc);
    connect(ui->btnDelete, &QPushButton::clicked, this, &mbClientDialogScannerRequest::deleteFunc);

    connect(ui->btnIcoAdd     , &QPushButton::clicked, this, &mbClientDialogScannerRequest::addFunc     );
    connect(ui->btnIcoModify  , &QPushButton::clicked, this, &mbClientDialogScannerRequest::modifyFunc  );
    connect(ui->btnIcoDelete  , &QPushButton::clicked, this, &mbClientDialogScannerRequest::deleteFunc  );
    connect(ui->btnIcoMoveUp  , &QPushButton::clicked, this, &mbClientDialogScannerRequest::moveUpFunc  );
    connect(ui->btnIcoMoveDown, &QPushButton::clicked, this, &mbClientDialogScannerRequest::moveDownFunc);
    connect(ui->btnIcoClear   , &QPushButton::clicked, this, &mbClientDialogScannerRequest::clearFuncs  );

    connect(ui->btnAddFileRecord     , &QPushButton::clicked, this, &mbClientDialogScannerRequest::addFileRecord     );
    connect(ui->btnDeleteFileRecord  , &QPushButton::clicked, this, &mbClientDialogScannerRequest::deleteFileRecord  );
    connect(ui->btnMoveUpFileRecord  , &QPushButton::clicked, this, &mbClientDialogScannerRequest::moveUpFileRecord  );
    connect(ui->btnMoveDownFileRecord, &QPushButton::clicked, this, &mbClientDialogScannerRequest::moveDownFileRecord);
    connect(ui->btnClearFileRecords  , &QPushButton::clicked, this, &mbClientDialogScannerRequest::clearFileRecords  );

}

mbClientDialogScannerRequest::~mbClientDialogScannerRequest()
{
    delete ui;
}

MBSETTINGS mbClientDialogScannerRequest::cachedSettings() const
{
    MBSETTINGS m;
    const Strings &s = Strings::instance();

    mbClientMessageParams params;
    params.setFileRecords(getCurrentFileRecords());

    m[s.func          ] = getCurrentFuncNum();
    m[s.subfunc       ] = getCurrentDiagnSubfuncNum();
    m[s.offset1       ] = ui->spOffset1        ->value      ();
    m[s.offset2       ] = ui->spOffset2        ->value      ();
    m[s.count2        ] = ui->spCount2         ->value      ();
    m[s.fileRecords   ] = params.fileRecordsAsByteArray();
    m[s.wGeometry     ] = this->saveGeometry();
    m[s.wSplitterState] = ui->splitter->saveState();

    return m;
}

void mbClientDialogScannerRequest::setCachedSettings(const MBSETTINGS &m)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    //bool ok;

    it = m.find(s.func          ); if (it != end) setCurrentFuncNum(static_cast<uint8_t>(it.value().toUInt()));
    it = m.find(s.subfunc       ); if (it != end) setCurrentDiagnSubfuncNum(static_cast<uint16_t>(it.value().toUInt()));
    it = m.find(s.offset1       ); if (it != end) ui->spOffset1        ->setValue       (it.value().toInt()   );
    it = m.find(s.offset2       ); if (it != end) ui->spOffset2        ->setValue       (it.value().toInt()   );
    it = m.find(s.count2        ); if (it != end) ui->spCount2         ->setValue       (it.value().toInt()   );
    it = m.find(s.fileRecords   ); if (it != end) {
        mbClientMessageParams params;
        params.setFileRecords(it.value().toByteArray());
        setCurrentFileRecords(params.fileRecords());
    }
    it = m.find(s.wGeometry     ); if (it != end) this                 ->restoreGeometry(it.value().toByteArray());
    it = m.find(s.wSplitterState); if (it != end) ui->splitter         ->restoreState   (it.value().toByteArray());
}

bool mbClientDialogScannerRequest::getRequest(mbClientScanner::Request_t &req)
{
    m_model->setRequest(req);
    if (req.count())
        ui->lsRequest->selectionModel()->select(m_model->index(0), QItemSelectionModel::Select);
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        req = m_model->request();
        return true;
    }
    return false;
}

void mbClientDialogScannerRequest::setFunction(const QModelIndex &current)
{
    mbClientMessageParams func = m_model->func(current.row());
    setCurrentFunc(func);
}

void mbClientDialogScannerRequest::addFunc()
{
    mbClientMessageParams func = getCurrentFunc();
    m_model->addFunc(func);
}

void mbClientDialogScannerRequest::modifyFunc()
{
    QModelIndexList ls = ui->lsRequest->selectionModel()->selectedRows();
    if (ls.count())
    {
        mbClientMessageParams func = getCurrentFunc();
        m_model->modifyFunc(ls.first().row(), func);
    }
}

void mbClientDialogScannerRequest::deleteFunc()
{
    QModelIndexList ls = ui->lsRequest->selectionModel()->selectedRows();
    if (ls.count())
    {
        m_model->deleteFunc(ls.first().row());
    }
}

void mbClientDialogScannerRequest::moveUpFunc()
{
    QModelIndexList ls = ui->lsRequest->selectionModel()->selectedRows();
    if (ls.count())
    {
        m_model->moveUpFunc(ls.first().row());
    }
}

void mbClientDialogScannerRequest::moveDownFunc()
{
    QModelIndexList ls = ui->lsRequest->selectionModel()->selectedRows();
    if (ls.count())
    {
        m_model->moveDownFunc(ls.first().row());
    }
}

void mbClientDialogScannerRequest::clearFuncs()
{
    m_model->clearFuncs();
}

void mbClientDialogScannerRequest::setCurrentFuncIndex(int i)
{
    uint8_t funcNum = m_funcNums.value(i);
    setCurrentFuncNum(funcNum);
}

void mbClientDialogScannerRequest::addFileRecord()
{
    auto itemFile = new QTableWidgetItem(QString("0"));
    auto itemRecord = new QTableWidgetItem(QString("0"));
    auto itemLen = new QTableWidgetItem(QString("1"));
    int i = ui->tblFileRecord->rowCount();
    ui->tblFileRecord->insertRow(i);
    ui->tblFileRecord->setItem(i, 0, itemFile);
    ui->tblFileRecord->setItem(i, 1, itemRecord);
    ui->tblFileRecord->setItem(i, 2, itemLen);
}

void mbClientDialogScannerRequest::deleteFileRecord()
{
    int i = ui->tblFileRecord->currentRow();
    if (i >= 0)
        ui->tblFileRecord->removeRow(i);
}

void mbClientDialogScannerRequest::moveUpFileRecord()
{
    int i = ui->tblFileRecord->currentRow();
    if (i > 0)
    {
        ui->tblFileRecord->insertRow(i - 1);
        for (int j = 0; j < ui->tblFileRecord->columnCount(); ++j)
        {
            ui->tblFileRecord->setItem(i - 1, j, ui->tblFileRecord->takeItem(i + 1, j));
        }
        ui->tblFileRecord->removeRow(i + 1);
        ui->tblFileRecord->setCurrentCell(i - 1, 0);
    }
}

void mbClientDialogScannerRequest::moveDownFileRecord()
{
    int i = ui->tblFileRecord->currentRow();
    if (i >= 0 && i < ui->tblFileRecord->rowCount() - 1)
    {
        ui->tblFileRecord->insertRow(i + 2);
        for (int j = 0; j < ui->tblFileRecord->columnCount(); ++j)
        {
            ui->tblFileRecord->setItem(i + 2, j, ui->tblFileRecord->takeItem(i, j));
        }
        ui->tblFileRecord->removeRow(i);
        ui->tblFileRecord->setCurrentCell(i + 1, 0);
    }
}

void mbClientDialogScannerRequest::clearFileRecords()
{
    ui->tblFileRecord->setRowCount(0);
}

mbClientMessageParams mbClientDialogScannerRequest::getCurrentFunc() const
{
    uint8_t funcNum = getCurrentFuncNum();
    mbClientMessageParams func;
    switch(funcNum)
    {
    case MBF_READ_COILS:
    case MBF_READ_DISCRETE_INPUTS:
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
    case MBF_WRITE_MULTIPLE_COILS:
    case MBF_WRITE_MULTIPLE_REGISTERS:
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        func.setFunction(funcNum);
        func.setOffset(static_cast<uint16_t>(ui->spOffset2->value()));
        func.setCount(static_cast<uint16_t>(ui->spCount2 ->value()));
        break;
    case MBF_WRITE_SINGLE_COIL:
    case MBF_WRITE_SINGLE_REGISTER:
    case MBF_MASK_WRITE_REGISTER:
    case MBF_READ_FIFO_QUEUE:
        func.setFunction(funcNum);
        func.setOffset(static_cast<uint16_t>(ui->spOffset1->value()));
        func.setCount(1);
        break;
    case MBF_READ_EXCEPTION_STATUS:
    case MBF_REPORT_SERVER_ID:
    case MBF_GET_COMM_EVENT_COUNTER:
    case MBF_GET_COMM_EVENT_LOG:
        func.setFunction(funcNum);
        break;
    case MBF_DIAGNOSTICS:
        func.setFunction(funcNum);
        func.setSubfunction(getCurrentDiagnSubfuncNum());
        break;
    case MBF_READ_FILE_RECORD:
    case MBF_WRITE_FILE_RECORD:
    {
        func.setFunction(funcNum);
        func.setFileRecords(getCurrentFileRecords());
    }
        break;
    case MBF_ENCAPSULATED_INTERFACE_TRANSPORT:
        func.setFunction(funcNum);
        func.setDeviceId(static_cast<uint8_t>(ui->spDeviceId->value()));
        func.setObjectId(static_cast<uint8_t>(ui->spObjectId->value()));
        break;
    }
    return func;
}

void mbClientDialogScannerRequest::setCurrentFunc(const mbClientMessageParams &f)
{
    int index = m_funcNums.indexOf(f.function());
    if (index < 0)
        return;
    switch(f.function())
    {
    case MBF_READ_COILS:
    case MBF_READ_DISCRETE_INPUTS:
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
    case MBF_WRITE_MULTIPLE_COILS:
    case MBF_WRITE_MULTIPLE_REGISTERS:
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        ui->spOffset2->setValue(f.offset());
        ui->spCount2 ->setValue(f.count());
        break;
    case MBF_WRITE_SINGLE_COIL:
    case MBF_WRITE_SINGLE_REGISTER:
    case MBF_MASK_WRITE_REGISTER:
    case MBF_READ_FIFO_QUEUE:
        ui->spOffset1->setValue(f.offset());
        break;
    case MBF_READ_EXCEPTION_STATUS:
    case MBF_REPORT_SERVER_ID:
    case MBF_GET_COMM_EVENT_COUNTER:
    case MBF_GET_COMM_EVENT_LOG:
        break;
    case MBF_DIAGNOSTICS:
        setCurrentDiagnSubfuncNum(f.subfunction());
        break;
    case MBF_READ_FILE_RECORD:
    case MBF_WRITE_FILE_RECORD:
        setCurrentFileRecords(f.fileRecords());
        break;
    case MBF_ENCAPSULATED_INTERFACE_TRANSPORT:
        ui->spDeviceId->setValue(f.deviceId());
        ui->spObjectId->setValue(f.objectId());
        break;
    default:
        return;
    }
    ui->cmbFunction->setCurrentIndex(index);
}

uint8_t mbClientDialogScannerRequest::getCurrentFuncNum() const
{
    return m_funcNums.value(ui->cmbFunction->currentIndex());
}

uint16_t mbClientDialogScannerRequest::getCurrentDiagnSubfuncNum() const
{
    return m_diagnSubfuncNums.value(ui->cmbDiagnSubfunction->currentIndex());
}

void mbClientDialogScannerRequest::setCurrentFuncNum(uint8_t funcNum)
{
    if (getCurrentFuncNum() != funcNum)
    {
        int index = m_funcNums.indexOf(funcNum);
        if (index < 0)
            return;
        ui->cmbFunction->setCurrentIndex(index);
    }
    switch(funcNum)
    {
    case MBF_READ_COILS:
    case MBF_READ_DISCRETE_INPUTS:
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
    case MBF_WRITE_MULTIPLE_COILS:
    case MBF_WRITE_MULTIPLE_REGISTERS:
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        ui->swFuncParams->setCurrentWidget(ui->pgOffsetCount);
        break;
    case MBF_WRITE_SINGLE_COIL:
    case MBF_WRITE_SINGLE_REGISTER:
    case MBF_MASK_WRITE_REGISTER:
    case MBF_READ_FIFO_QUEUE:
        ui->swFuncParams->setCurrentWidget(ui->pgOffset);
        break;
    case MBF_DIAGNOSTICS:
        ui->swFuncParams->setCurrentWidget(ui->pgDiagn);
        break;
    case MBF_READ_FILE_RECORD:
    case MBF_WRITE_FILE_RECORD:
        ui->swFuncParams->setCurrentWidget(ui->pgFileRecord);
        break;
    case MBF_ENCAPSULATED_INTERFACE_TRANSPORT:
        ui->swFuncParams->setCurrentWidget(ui->pgReadDeviceId);
        break;
    default:
        ui->swFuncParams->setCurrentWidget(ui->pgEmpty);
        break;
    }
}

void mbClientDialogScannerRequest::setCurrentDiagnSubfuncNum(uint16_t subfunc)
{
    int i = 0;
    Q_FOREACH (int f, m_diagnSubfuncNums)
    {
        if (f == subfunc)
        {
            ui->cmbDiagnSubfunction->setCurrentIndex(i);
            break;
        }
        ++i;
    }
}

QVector<Modbus::FileRecord> mbClientDialogScannerRequest::getCurrentFileRecords() const
{
    QVector<Modbus::FileRecord> fileRecords;
    fileRecords.resize(ui->tblFileRecord->rowCount());
    for (int i = 0; i < ui->tblFileRecord->rowCount(); ++i)
    {
        uint16_t fileNum = static_cast<uint16_t>(ui->tblFileRecord->item(i, 0)->text().toUInt());
        uint16_t recordNum = static_cast<uint16_t>(ui->tblFileRecord->item(i, 1)->text().toUInt());
        uint16_t recordLen = static_cast<uint16_t>(ui->tblFileRecord->item(i, 2)->text().toUInt());
        fileRecords[i] = {fileNum, recordNum, recordLen};
    }
    return fileRecords;
}

void mbClientDialogScannerRequest::setCurrentFileRecords(const QVector<Modbus::FileRecord> &fileRecords)
{
    ui->tblFileRecord->setRowCount(fileRecords.size());
    for (int i = 0; i < fileRecords.size(); ++i)
    {
        const auto &fr = fileRecords[i];
        auto itemFile = new QTableWidgetItem(QString::number(fr.fileNumber));
        auto itemRecord = new QTableWidgetItem(QString::number(fr.recordNumber));
        auto itemLen = new QTableWidgetItem(QString::number(fr.recordLength));
        ui->tblFileRecord->setItem(i, 0, itemFile);
        ui->tblFileRecord->setItem(i, 1, itemRecord);
        ui->tblFileRecord->setItem(i, 2, itemLen);
    }
}

/* =============================================================
 * =========================== MODEL ===========================
 */
int mbClientDialogScannerRequest::Model::rowCount(const QModelIndex &) const
{
    return m_req.count();
}

QVariant mbClientDialogScannerRequest::Model::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return mbClientScanner::toString(m_req.value(index.row()));
    return QVariant();
}

void mbClientDialogScannerRequest::Model::setRequest(const mbClientScanner::Request_t &req)
{
    beginResetModel();
    m_req = req;
    endResetModel();
}

mbClientMessageParams mbClientDialogScannerRequest::Model::func(int i)
{
    return m_req.value(i);
}

void mbClientDialogScannerRequest::Model::addFunc(const mbClientMessageParams &f)
{
    int c = m_req.count();
    beginInsertRows(QModelIndex(), c, c);
    m_req.append(f);
    endInsertRows();
}

void mbClientDialogScannerRequest::Model::modifyFunc(int i, const mbClientMessageParams &f)
{
    if ((0 <= i) && (i < m_req.size()))
    {
        m_req[i] = f;
        QModelIndex ind = index(i);
        Q_EMIT dataChanged(ind, ind);
    }
}

void mbClientDialogScannerRequest::Model::deleteFunc(int i)
{
    if ((0 <= i) && (i < m_req.size()))
    {
        beginRemoveRows(QModelIndex(), i, i);
        m_req.removeAt(i);
        endRemoveRows();
    }
}

void mbClientDialogScannerRequest::Model::moveUpFunc(int i)
{
    if ((0 < i) && (i < m_req.size()))
    {
        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
        m_req.move(i, i-1);
        endMoveRows();
    }
}

void mbClientDialogScannerRequest::Model::moveDownFunc(int i)
{
    if ((0 <= i) && (i < m_req.size() - 1))
    {
        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i+2);
        m_req.move(i, i+1);
        endMoveRows();
    }
}

void mbClientDialogScannerRequest::Model::clearFuncs()
{
    beginResetModel();
    m_req.clear();
    endResetModel();
}

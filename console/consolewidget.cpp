#include "consolewidget.h"

#include "ui_consolewidget.h"

ConsoleWidget::ConsoleWidget(QWidget* parent, XComm* xcomm)
  : QWidget(parent)
  , ui(new Ui::ConsoleWidget)
  , m_xcomm(xcomm)
  , m_updateStatsTimer(new QTimer(this))
{
  ui->setupUi(this);

  initTextEditor();
  initLcd();
  initConnections();

  m_updateStatsTimer->start(updateCommStatsInterval);
}

ConsoleWidget::~ConsoleWidget()
{
  delete ui;
}

void
ConsoleWidget::initTextEditor()
{
  // send&recv文本框灰底绿字
  ui->cmdDataTextEdit->setStyleSheet(
    QStringLiteral("QTextEdit{background:#808080;color:#00ff00;}"));
  ui->recvTextEdit->setStyleSheet(
    QStringLiteral("QTextEdit{background:#808080;color:#00ff00;}"));
  // log文本框灰底彩色字
  ui->logTextEdit->setStyleSheet(
    QStringLiteral("QTextEdit{background:#808080;}"));
}

void
ConsoleWidget::initLcd()
{
  /*
   * 设置颜色，整体背景颜色
   * 颜色蓝色,此函数的第一个参数可以设置多种。如文本、按钮按钮文字、多种
   */
  //背景灰色
  // requestLcdNumber文本绿色
  ui->requestLcdNumber->setStyleSheet(
    QStringLiteral("background:#000000;color:#23DE53;"));
  // errorLcdNumber文本红色
  ui->errorLcdNumber->setStyleSheet(
    QStringLiteral("background:#000000;color:#FF0000;"));
  // errorLcdNumber文本黄色
  ui->warnLcdNumber->setStyleSheet(
    QStringLiteral("background:#000000;color:#FFFF00;"));
  // avgTimeLcdNumber文本蓝色
  ui->avgTimeLcdNumber->setStyleSheet(
    QStringLiteral("background:#000000;color:#00CCFF;"));
  // txdLcdNumber文本蓝色
  ui->txdLcdNumber->setStyleSheet(
    QStringLiteral("background:#000000;color:#99FFFF;"));
  // rxdLcdNumber文本蓝色
  ui->rxdLcdNumber->setStyleSheet(
    QStringLiteral("background:#000000;color:#99FFFF;"));
}

void
ConsoleWidget::initConnections()
{
  connect(
    m_xcomm, &XComm::sendCommLog, this, &ConsoleWidget::slotProcessCommLog);
  connect(m_updateStatsTimer,
          &QTimer::timeout,
          this,
          &ConsoleWidget::slotUpdateStats);
}

void
ConsoleWidget::slotProcessCommLog(AbstractComm::LogLevel level,
                                  quint16 errCmdInt,
                                  const QString& msgLog)
{
  //看不见不更新
  if (!isVisible()) {
    return;
  }
  // >maxEditorLine行清理文本框
  QTextDocument* textDoc = ui->logTextEdit->document();
  if (textDoc->lineCount() > maxEditorLine) {
    ui->logTextEdit->clear();
  }
  //报错对应的CMD
  //消息等级
  //不同等级不同颜色
  switch (level) {
    case AbstractComm::MSG_OK:
      // holdplace
      break;
    case AbstractComm::MSG_INFO:
      // just info, also append on warnLogTextEdit
      ui->logTextEdit->append(tr("<font color=\"#00CCFF\">CMD:%1 %2</font>")
                                .arg(errCmdInt, 0, 16)
                                .arg(msgLog));
      break;
    case AbstractComm::MSG_WARNING:
      ui->logTextEdit->append(tr("<font color=\"#FFFF00\">CMD:%1 %2</font>")
                                .arg(errCmdInt, 0, 16)
                                .arg(msgLog));
      break;
    case AbstractComm::MSG_ERROR:
      ui->logTextEdit->append(tr("<font color=\"#FF0000\">CMD:%1 %2</font>")
                                .arg(errCmdInt, 0, 16)
                                .arg(msgLog));
      break;
    default:
      break;
  }
}

void
ConsoleWidget::slotUpdateStats()
{
  //看不见不更新
  if (!isVisible()) {
    return;
  }
  //更新通讯状态计数器
  ui->requestLcdNumber->display(m_xcomm->getStats().m_cmdCnt);
  ui->warnLcdNumber->display(m_xcomm->getStats().m_warnCnt);
  ui->errorLcdNumber->display(m_xcomm->getStats().m_errCnt);
  ui->avgTimeLcdNumber->display(m_xcomm->getStats().m_avgElapsedTime);
  auto txdBytes = static_cast<double>(m_xcomm->getStats().m_totalTxdBytes);
  ui->txdLcdNumber->display(txdBytes);
  auto rxdBytes = static_cast<double>(m_xcomm->getStats().m_totalRxdBytes);
  ui->rxdLcdNumber->display(rxdBytes);
}

void
ConsoleWidget::slotShowCmd(const quint16 cmd, const QByteArray& data)
{
  if (!isVisible()) {
    return;
  }
  // >maxEditorLine行清理文本框
  QTextDocument* recvDoc = ui->recvTextEdit->document();
  if (recvDoc->lineCount() > maxEditorLine) {
    ui->logTextEdit->clear();
  }
  // 16表示16进制显示
  ui->recvTextEdit->append(tr("cmd: %1, respon: %2")
                             .arg(cmd, 0, 16)
                             .arg(QString(data.toHex(':')), 0, 16));
}

void
ConsoleWidget::on_requestLcdNumber_overflow()
{
  ui->requestLcdNumber->display(0);
  m_xcomm->clearCmdCnt();
}

void
ConsoleWidget::on_warnLcdNumber_overflow()
{
  ui->warnLcdNumber->display(0);
  m_xcomm->clearWarnCnt();
}

void
ConsoleWidget::on_errorLcdNumber_overflow()
{
  ui->errorLcdNumber->display(0);
  m_xcomm->clearErrCnt();
}

void
ConsoleWidget::on_avgTimeLcdNumber_overflow()
{
  ui->avgTimeLcdNumber->display(0);
  m_xcomm->clearTotalTimeElapsed();
}

void
ConsoleWidget::on_txdLcdNumber_overflow()
{
  ui->txdLcdNumber->display(0);
  m_xcomm->clearTotalTxdBytes();
}

void
ConsoleWidget::on_rxdLcdNumber_overflow()
{
  ui->rxdLcdNumber->display(0);
  m_xcomm->clearTotalRxdBytes();
}

void
ConsoleWidget::on_sendButton_clicked()
{
  if (m_xcomm->connectStatus() == XComm::COMM_CONNECT) {
    //提取命令选项
    quint16 cmd = ui->cmdComboBox->currentText().toUShort(nullptr, 16);
    //命令行参数
    QByteArray cmdData;
    bool isOk = true;
    if (!ui->cmdDataTextEdit->document()->isEmpty()) {
      //命令参数不为空，提取命令参数文本,转为字节数据
      QString dataStr = ui->cmdDataTextEdit->toPlainText();
      QStringList dataStrList = dataStr.split(QStringLiteral(" "));
      Q_FOREACH (const auto& str, dataStrList) {
        auto dataByte = static_cast<quint8>(str.toInt(&isOk, 16));
        if (!isOk) { //转换成字节数据失败
          break;
        }
        //转换成字节数据成功
        cmdData.append(dataByte);
      }
    }
    if (isOk) {
      //成功
      m_xcomm->command(cmd, cmdData);
    } else {
      QMessageBox::warning(
        this, QStringLiteral("错误"), QStringLiteral("参数格式错误"));
    }
  } else { // COMM_IDLE
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("未连接驱动器，请先连接"));
  }
}

void
ConsoleWidget::on_clearLogButton_clicked()
{
  ui->logTextEdit->clear();
}

void
ConsoleWidget::on_clearCmdPushButton_clicked()
{
  ui->cmdDataTextEdit->clear();
}

void
ConsoleWidget::on_clearRecvPushButton_clicked()
{
  ui->recvTextEdit->clear();
}

void
ConsoleWidget::on_clearCounterPushButton_clicked()
{
  ui->requestLcdNumber->display(0);
  ui->warnLcdNumber->display(0);
  ui->errorLcdNumber->display(0);
  ui->avgTimeLcdNumber->display(0);
  ui->rxdLcdNumber->display(0);
  ui->txdLcdNumber->display(0);

  m_xcomm->clearCmdCnt();
  m_xcomm->clearErrCnt();
  m_xcomm->clearWarnCnt();
  m_xcomm->clearTotalTimeElapsed();
  m_xcomm->clearTotalRxdBytes();
  m_xcomm->clearTotalTxdBytes();
}

void
ConsoleWidget::on_showRecvCheckBox_stateChanged(int state)
{
  switch (state) {
    case Qt::Unchecked:
      disconnect(m_xcomm, &XComm::allCmd, this, &ConsoleWidget::slotShowCmd);
      break;
    case Qt::PartiallyChecked:
      break;
    case Qt::Checked:
      connect(m_xcomm, &XComm::allCmd, this, &ConsoleWidget::slotShowCmd);
      break;
    default:
      break;
  }
}

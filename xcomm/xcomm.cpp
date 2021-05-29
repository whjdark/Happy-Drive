/*
 * @Author: whj
 * @Date: 2021-02-01 20:27:39
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:05:13
 * @Description: file content
 */

#include "xcomm.h"

XComm::XComm(QObject* parent)
  : QObject(parent)
  , m_port(nullptr)
{}

void
XComm::setSlotConnect(bool isConnect)
{
  if (isConnect) {
    connect(
      m_port, &AbstractPort::signalResponse, this, &XComm::slotForwardResponse);
    connect(m_port, &AbstractPort::signalLog, this, &XComm::slotUpdateCommLog);
  } else {
    disconnect(
      m_port, &AbstractPort::signalResponse, this, &XComm::slotForwardResponse);
    disconnect(
      m_port, &AbstractPort::signalLog, this, &XComm::slotUpdateCommLog);
  }
}

void
XComm::command(const quint16 cmd, const QByteArray& data)
{
  if (m_commState == XComm::COMM_CONNECT || cmd == XComm::TRY_CONNECT) {
    // allow to command when  driver is CONNECT or TRY_CONNECT
    // TRY_CONNECT will send a test msg & try to connect DSP
    m_port->transaction(cmd, data);
    //更新统计信息
    m_commStats.m_cmdCnt++;
    m_commStats.m_totalTxdBytes += data.size() + sizeof(cmd);
  } else {
    // command but in DISCONNECTED, ignore
    // holdplace
  }
}

void
XComm::configPort(const Serial::SerialConfig& serialConfig)
{
  // abstractport factory produces serialport
  m_port = new Serial(this);
  // connect signal & slot after instantiation
  setSlotConnect(true);
  auto* serialPort = qobject_cast<Serial*>(m_port);
  serialPort->configSerial(serialConfig);
}

auto
XComm::getConnectStatus() const -> XComm::CommState
{
  return m_commState;
}

void
XComm::tryConnect()
{
  if (m_commState == XComm::COMM_IDLE) {
    //启动通讯
    m_port->openPort();
    command(XComm::TRY_CONNECT, QByteArray());
  } else {
    // holdplace
  }
}

void
XComm::disconnectDriver()
{
  m_port->closePort();
  m_commState = XComm::COMM_IDLE;
  m_motorState = XComm::MOTOR_STOP;
  // release m_port
  setSlotConnect(false); // disconnect signal & slot
  delete m_port;
  m_port = nullptr;
};

void
XComm::startMotor(DriverDataType::RunConfigType& runConfig)
{
  command(START_MOTOR, runConfig.toByteArray());
}

void
XComm::stopMotor()
{
  command(STOP_MOTOR, QByteArray());
}

auto
XComm::getCurRunModeStr() const -> QString
{
  QString runModeStr;
  switch (currentRunMode) {
    case DriverDataType::MODE0:
      runModeStr = QStringLiteral("模式0：电流开环");
      break;
    case DriverDataType::MODE1:
      runModeStr = QStringLiteral("模式1：速度闭环（电流PI，速度PI）");
      break;
    case DriverDataType::MODE2:
      runModeStr = QStringLiteral("模式2：位置闭环（位置P，电流PI，速度PI）");
      break;
    case DriverDataType::MODE3:
      runModeStr = QStringLiteral("模式3：速度闭环（速度ADRC，电流PI）");
      break;
    case DriverDataType::MODE4:
      runModeStr = QStringLiteral("模式4：速度闭环（速度ADRC，电流ADRC）");
      break;
    case DriverDataType::MODE5:
      runModeStr =
        QStringLiteral("模式5：位置闭环（位置P，速度ADRC，电流ADRC）");
      break;
    case DriverDataType::MODE6:
      runModeStr =
        QStringLiteral("模式6：速度闭环（电流环PI，速度环FOPD-GESO）");
      break;
    case DriverDataType::MODE_FRT_MECH:
      runModeStr = QStringLiteral("模式：频率响应测试（机械环节）");
      break;
    case DriverDataType::MODE_FRT_ELEC:
      runModeStr = QStringLiteral("模式：频率响应测试（电磁环节）");
      break;
    default:
      break;
  }
  return runModeStr;
}

void
XComm::slotForwardResponse(const quint16 cmd, const QByteArray& data)
{
  //事件转发
  int cmdHeader = cmd >> 8; // 提取高8位（命令头）用于消息转发
  switch (cmdHeader) {
    case XComm::SYSTEM_HEADER:
      slotSysCmd(cmd, data);
      break;
    case XComm::MONITOR_HEADER:
      Q_EMIT signalMonitorCmd(cmd, data);
      break;
    case XComm::TRACER_HEADER:
      Q_EMIT signalTracerCmd(cmd, data);
      break;
    case XComm::CONTROLLER_HEADER:
      Q_EMIT signalConfigerCmd(cmd, data);
      break;
    case XComm::TOOLBOX_FRT_HEADER:
      Q_EMIT signalFRTCmd(cmd, data);
      break;
    default:
      // unexpected cmd header
      m_commStats.m_errCnt++;
      QString errMsg = QStringLiteral("[ERROR]unexpected cmd header");
      Q_EMIT signalCommLog(AbstractPort::MSG_ERROR, 0, errMsg);
      break;
  }
  //更新统计信息
  m_commStats.m_totalRxdBytes += data.size() + sizeof(cmd);
  //发送所有消息给控制台，用于调试
  //太长的消息进行截断，方便展示分析
  QByteArray truncatedData;
  if (data.count() > 16) {
    truncatedData = data.mid(0, 16);
  } else {
    truncatedData = data;
  }
  Q_EMIT signalResponseLog(cmd, truncatedData);
}

void
XComm::slotSysCmd(const quint16 cmd, const QByteArray& data)
{
  switch (cmd) {
    case TRY_CONNECT:
      // driver responsed & connection is OK
      m_commState = COMM_CONNECT;
      slotUpdateCommLog(AbstractPort::MSG_OK,
                        QByteArray(),
                        QStringLiteral("[OK]connect success"));
      Q_EMIT signalConnectSuccess();
      break;
    case START_MOTOR:
      // driver responsed & start running
      m_motorState = MOTOR_RUN;
      Q_EMIT signalMotorStart();
      break;
    case XComm::STOP_MOTOR:
      // stop motor running
      m_motorState = MOTOR_STOP;
      Q_EMIT signalMotorStop();
      break;
    default:
      break;
  }
}

void
XComm::slotUpdateCommLog(AbstractPort::LogLevel level,
                         const QByteArray& cmd,
                         const QString& msgStr)
{
  switch (level) {
    case AbstractPort::MSG_OK:
      // holdplace
    case AbstractPort::MSG_INFO:
      // holdplace
      break;
    case AbstractPort::MSG_WARNING:
      m_commStats.m_warnCnt++;
      break;
    case AbstractPort::MSG_ERROR:
      m_commStats.m_errCnt++;
      break;
    default:
      break;
  }
  quint16 cmdInt = BitConverter::ba2Int(cmd);
  Q_EMIT signalCommLog(level, cmdInt, msgStr);
}

auto
XComm::getStats() -> const CommStats&
{
  // calculate average communication elapsed time
  // divide 0 will occur error
  if (m_port != nullptr) {
    // do not update when port is not init
    if (m_commStats.m_cmdCnt != 0) {
      quint64 totalTimeElapsed = m_port->getTotalTimeElapse();
      m_commStats.m_avgElapsedTime =
        static_cast<int>(totalTimeElapsed / (m_commStats.m_cmdCnt));
    }
  }
  return m_commStats;
}

void
XComm::clearCmdCnt()
{
  m_commStats.m_cmdCnt = 0;
}

void
XComm::clearErrCnt()
{
  m_commStats.m_errCnt = 0;
}

void
XComm::clearWarnCnt()
{
  m_commStats.m_warnCnt = 0;
}

void
XComm::clearTotalTimeElapsed()
{
  m_commStats.m_avgElapsedTime = 0;
  m_port->clearTotalTimeElapse();
}

void
XComm::clearTotalTxdBytes()
{
  m_commStats.m_totalTxdBytes = 0;
}

void
XComm::clearTotalRxdBytes()
{
  m_commStats.m_totalRxdBytes = 0;
}

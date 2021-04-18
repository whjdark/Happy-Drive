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
{
  initConnections();
}

void
XComm::initConnections()
{
  connect(&m_serial, &Serial::response, this, &XComm::slotResponse);
  connect(&m_serial, &Serial::commLog, this, &XComm::slotCommLog);
}

void
XComm::command(const quint16 cmd, const QByteArray& data)
{
  if (m_commState == XComm::COMM_CONNECT || cmd == XComm::TRY_CONNECT) {
    // allow to command when  driver is CONNECT or TRY_CONNECT
    // TRY_CONNECT will send a test msg & try to connect DSP
    m_serial.transaction(cmd, data);
    //更新统计信息
    m_commStats.m_cmdCnt++;
    m_commStats.m_totalTxdBytes += data.size() + sizeof(cmd);
  } else {
    // command but in DISCONNECTED, ignore
    // holdplace
  }
}

void
XComm::configSerialPort(const Serial::SerialConfig& serialConfig)
{
  m_serial.configSerial(serialConfig);
}

auto
XComm::getConnectStatus() const -> XComm::CommState
{
  return m_commState;
}

void
XComm::connectDriver()
{
  if (m_commState == XComm::COMM_IDLE) {
    //启动通讯
    m_serial.startComm();
    command(XComm::TRY_CONNECT, QByteArray());
  } else {
    // holdplace
  }
}

void
XComm::disconnectDriver()
{
  m_serial.closeComm();
  m_commState = XComm::COMM_IDLE;
  m_motorState = XComm::MOTOR_STOP;
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
XComm::getCurrentRunModeStr() const -> QString
{
  QString runModeStr;
  switch (currentRunMode) {
    case DriverDataType::MODE0:
      runModeStr = QStringLiteral("模式0：开环");
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
    case DriverDataType::MODE7:
      runModeStr = QStringLiteral("模式7：扫频（机械环节）");
      break;
    case DriverDataType::MODE8:
      runModeStr = QStringLiteral("模式8：扫频（电磁环节）");
      break;
    default:
      break;
  }
  return runModeStr;
}

void
XComm::slotResponse(const quint16 cmd, const QByteArray& data)
{
  //事件转发
  int cmdHeader = cmd >> 8; // 提取高8位（命令头）用于消息转发
  switch (cmdHeader) {
    case XComm::SYSTEM_HEADER:
      slotSysCmd(cmd, data);
      break;
    case XComm::MONITOR_HEADER:
      Q_EMIT monitorCmd(cmd, data);
      break;
    case XComm::TRACER_HEADER:
      Q_EMIT tracerCmd(cmd, data);
      break;
    case XComm::CONTROLLER_HEADER:
      Q_EMIT controllerCmd(cmd, data);
      break;
    case XComm::TOOLBOX_SWEEPING_HEADER:
      Q_EMIT toolboxSweepingCmd(cmd, data);
      break;
    default:
      // unexpected cmd header
      m_commStats.m_errCnt++;
      QString msgStr = QStringLiteral("[ERROR]unexpected cmd header");
      Q_EMIT sendCommLog(AbstractComm::MSG_ERROR, 0, msgStr);
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
  Q_EMIT logAllRecv(cmd, truncatedData);
}

void
XComm::slotSysCmd(const quint16 cmd, const QByteArray& data)
{
  Q_UNUSED(data);
  switch (cmd) {
    case XComm::TRY_CONNECT:
      // driver responsed & connection is OK
      m_commState = XComm::COMM_CONNECT;
      Q_EMIT connectSuccess();
      break;
    case XComm::START_MOTOR:
      // driver responsed & start running
      m_motorState = XComm::MOTOR_RUN;
      Q_EMIT motorStart();
      break;
    case XComm::STOP_MOTOR:
      // stop motor running
      m_motorState = XComm::MOTOR_STOP;
      Q_EMIT motorStop();
      break;
    default:
      break;
  }
}

void
XComm::slotCommLog(AbstractComm::LogLevel level,
                   const QByteArray& errCmd,
                   const QString& msgStr)
{
  switch (level) {
    case AbstractComm::MSG_OK:
      // holdplace
    case AbstractComm::MSG_INFO:
      // just info
      break;
    case AbstractComm::MSG_WARNING:
      m_commStats.m_warnCnt++;
      break;
    case AbstractComm::MSG_ERROR:
      m_commStats.m_errCnt++;
      break;
    default:
      break;
  }
  quint16 errCmdInt = BitConverter::ba2Int(errCmd);
  Q_EMIT sendCommLog(level, errCmdInt, msgStr);
}

auto
XComm::getStats() -> const CommStats&
{
  // calculate average communication elapsed time
  // divide 0 will occur error
  if (m_commStats.m_cmdCnt != 0) {
    quint64 totalTimeElapsed = m_serial.getTotalTimeElapse();
    m_commStats.m_avgElapsedTime =
      static_cast<int>(totalTimeElapsed / (m_commStats.m_cmdCnt));
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
  m_serial.clearTotalTimeElapse();
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

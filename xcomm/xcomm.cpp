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
  , m_updateTimer(new QTimer(this))
{
  // only for Tcp Client
  connect(m_updateTimer, &QTimer::timeout, this, [=]() {
    // send something to host
    // To request EtherCAT server data
    command(0x0000, QByteArray("HELLO"));
  });
}

void
XComm::setSlotConnect(bool isConnect)
{
  using AB = AbstractPort;
  if (isConnect) {
    connect(m_port, &AB::signalResponse, this, &XComm::slotForwardResponse);
    connect(m_port, &AB::signalLog, this, &XComm::slotUpdateCommLog);
  } else {
    disconnect(m_port, &AB::signalResponse, this, &XComm::slotForwardResponse);
    disconnect(m_port, &AB::signalLog, this, &XComm::slotUpdateCommLog);
  }
}

void
XComm::command(const quint16 cmd, const QByteArray& data)
{
  if (m_port == nullptr) {
    return;
  }
  if (m_commStatus == COMM_CONNECT || cmd == TRY_CONNECT) {
    // allow to command when  driver is in CONNECT or TRY_CONNECT
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

auto
XComm::getPortType() const -> AbstractPort::PortType
{
  return m_port->getPortType();
}

void
XComm::configPort(const Serial::SerialConfig& Config)
{
  // abstractport factory produces serialport
  m_port = new Serial(this);
  // connect signal & slot after instantiation
  setSlotConnect(true);
  auto* serialPort = qobject_cast<Serial*>(m_port);
  serialPort->configSerial(Config);
}

void
XComm::configPort(const TcpClient::TCPConfig& config)
{
  // abstractport factory produces serialport
  m_port = new TcpClient(this);
  // connect signal & slot after instantiation
  setSlotConnect(true);
  auto* tcpClient = qobject_cast<TcpClient*>(m_port);
  tcpClient->configClient(config);
}

auto
XComm::getCommStatus() const -> XComm::CommState
{
  return m_commStatus;
}

void
XComm::tryConnectDriver()
{
  if (m_commStatus == COMM_IDLE) {
    //启动通讯
    switch (m_port->getPortType()) {
      case AbstractPort::SerialPort:
        m_port->openPort();
        // TRY_CONNECT cmd is designed for serial
        command(TRY_CONNECT, QByteArray());
        break;
      case AbstractPort::TCPClient:
        m_port->openPort();
        break;
      default:
        break;
    }
  } else {
    // holdplace
  }
}

void
XComm::disconnectDriver()
{
  m_commStatus = COMM_IDLE;
  m_motorStatus = MOTOR_STOP;
  m_updateTimer->stop(); // stop update. only for TCP
  // release m_port
  m_port->closePort();
  // disconnect signal & slot , should i do it?
  setSlotConnect(false);
  m_port->deleteLater(); // instead of delete
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
    case DriverDataType::MODE7:
      runModeStr =
        QStringLiteral("模式7：位置闭环（位置P，电流环PI，速度环FOPD-GESO）");
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
  if (m_port->getPortType() == AbstractPort::SerialPort) {
    //事件转发 为SerialPort设计
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
  } else if (m_port->getPortType() == AbstractPort::TCPClient) {
    //直接处理事件 为ctp client主站通讯设计
    if (cmd == 0x1001) {
      // successfully connect to host, info user
      m_commStatus = COMM_CONNECT;
      slotUpdateCommLog(AbstractPort::MSG_OK,
                        QByteArray(),
                        QStringLiteral("[OK]connect success"));
      Q_EMIT signalConnectSuccess();
      m_updateTimer->start(50); // only available for Tcp client
    } else if (cmd == 0x000) {
      // byte array to etherCAT data struct
      m_eCATData.ba2Struct(data);
    }
  }
}

void
XComm::slotSysCmd(const quint16 cmd, const QByteArray& data)
{
  Q_UNUSED(data);
  switch (cmd) {
    case TRY_CONNECT:
      // driver responsed & connection is OK
      m_commStatus = COMM_CONNECT;
      slotUpdateCommLog(AbstractPort::MSG_OK,
                        QByteArray(),
                        QStringLiteral("[OK]connect success"));
      Q_EMIT signalConnectSuccess();
      break;
    case START_MOTOR:
      // driver responsed & start running
      m_motorStatus = MOTOR_RUN;
      Q_EMIT signalMotorStart();
      break;
    case XComm::STOP_MOTOR:
      // stop motor running
      m_motorStatus = MOTOR_STOP;
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
      // Error occurs ,stop communication
      disconnectDriver();
      break;
    default:
      break;
  }
  // some error messages do not have cmd string
  quint16 cmdInt = 0;
  if (!cmd.isEmpty()) {
    cmdInt = BitConverter::ba2Int(cmd);
  }
  Q_EMIT signalCommLog(level, cmdInt, msgStr);
}

auto
XComm::getStats() -> const CommStats&
{
  // do not update when port is not exist
  if (m_port == nullptr) {
    return m_commStats;
  }
  if (m_port->getPortType() == AbstractPort::SerialPort) {
    // only support SerialPort for NOW
    if (m_commStats.m_cmdCnt != 0) {
      quint64 totalTimeElapsed = m_port->getTotalTimeElapse();
      // calculate average communication elapsed time
      // divide 0 will occur error
      m_commStats.m_avgElapsedTime =
        static_cast<int>(totalTimeElapsed / (m_commStats.m_cmdCnt));
    }
  }
  return m_commStats;
}

void
XComm::setMotorState(const MotorState motorState)
{
  m_motorStatus = motorState;
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

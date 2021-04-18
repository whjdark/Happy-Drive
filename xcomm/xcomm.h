/*
 * @Author: whj
 * @Date: 2021-02-01 20:27:39
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:05:17
 * @Description: file content
 */

#ifndef XCOMM_H
#define XCOMM_H

#include "../BaseDataType/bitconverter.h"
#include "../BaseDataType/driverdatatype.h"
#include "serial/serialcomm.h"

#include <QByteArray>
#include <QQueue>
#include <QString>

class XComm : public QObject
{
  Q_OBJECT

public:
  enum MotorState
  {
    MOTOR_STOP = 0,
    MOTOR_RUN = 1,
  };
  Q_ENUM(MotorState)

  enum CommState
  {
    COMM_IDLE = 0,
    COMM_CONNECT = 1,
  };
  Q_ENUM(CommState)

  enum CmdHeaderType
  {
    SYSTEM_HEADER = 0x00,
    MONITOR_HEADER = 0x10,
    TRACER_HEADER = 0x20,
    CONTROLLER_HEADER = 0x30,
    TOOLBOX_SWEEPING_HEADER = 0x40,

    HEADER_UNKNOWN = 0xFF,
  };
  Q_ENUM(CmdHeaderType)

  enum CmdType
  {
    TRY_CONNECT = 0x0001,
    START_MOTOR = 0x0002,
    STOP_MOTOR = 0x0003,

    MONITOR_REQ_DATA = 0x1001,

    TRACER_REQ_VELOCITY_FDB = 0x2001,  //速度反馈值
    TRACER_REQ_VELOCITY_REF = 0x2002,  //速度给定值
    TRACER_REQ_VELOCITY_OUT = 0x2003,  //速度调节器输出值
    TRACER_REQ_CURRENT_FDB = 0x2004,   //电流反馈值
    TRACER_REQ_CURRENT_REF = 0x2005,   //电流给定值
    TRACER_REQ_CURRENT_OUT = 0x2006,   //电流调节器输出值
    TRACER_REQ_POSITION_FDB = 0x2007,  //位置反馈值
    TRACER_REQ_POSITION_REF = 0x2008,  //位置给定值
    TRACER_REQ_POSITION_OUT = 0x2009,  //位置调节器输出值
    TRACER_REQ_ENCODER_CLOCK = 0x200A, //编码器采集数据时钟数值
    TRACER_REQ_CURRENT_U = 0x200B,     // U相电流
    TRACER_REQ_CURRENT_V = 0x200C,     // V相电流
    TRACER_REQ_CURRENT_W = 0x200D,     // W相电流

    CONTROLLER_WRITE = 0x3001,
    CONTROLLER_READ = 0x3002,

    TOOLBOX_SWEEPING_WRITE = 0x4001,
    TOOLBOX_SWEEPING_REQ_AM = 0x4002,
    TOOLBOX_SWEEPING_REQ_PH = 0x4003,

    CMD_UNKNOWN = 0xFFFF,
  };
  Q_ENUM(CmdType)

  struct CommStats
  {
    int m_cmdCnt;
    int m_errCnt;
    int m_warnCnt;
    int m_avgElapsedTime;
    quint64 m_totalTxdBytes;
    quint64 m_totalRxdBytes;

    CommStats()
      : m_cmdCnt(0)
      , m_errCnt(0)
      , m_warnCnt(0)
      , m_avgElapsedTime(0)
      , m_totalTxdBytes(0)
      , m_totalRxdBytes(0)
    {}
  };

public:
  explicit XComm(QObject* parent = nullptr);
  ~XComm() = default;

  void initConnections();
  void command(const quint16 cmd, const QByteArray& data);
  const QString& getCurrentSerialPort() const { return m_serial.currentPort(); }
  void configSerialPort(const Serial::SerialConfig& serialConfig);
  void connectDriver();
  void disconnectDriver();
  CommState getConnectStatus() const;
  const CommStats& getStats();
  MotorState getMotorStatus() const { return m_motorState; }
  void setMotorState(const MotorState motorState) { m_motorState = motorState; }
  void clearCmdCnt();
  void clearErrCnt();
  void clearWarnCnt();
  void clearTotalTimeElapsed();
  void clearTotalTxdBytes();
  void clearTotalRxdBytes();
  void startMotor(DriverDataType::RunConfigType& runConfig);
  void stopMotor();
  DriverDataType::RunMode getCurrentRunMode() const { return currentRunMode; }
  QString getCurrentRunModeStr() const;
  void logRunMode(const DriverDataType::RunMode mode) { currentRunMode = mode; }

Q_SIGNALS:
  void connectSuccess();
  void motorStart();
  void motorStop();
  void logAllRecv(const quint16 cmd, const QByteArray& data);
  void monitorCmd(const quint16 cmd, const QByteArray& data);
  void tracerCmd(const quint16 cmd, const QByteArray& data);
  void controllerCmd(const quint16 cmd, const QByteArray& data);
  void toolboxSweepingCmd(const quint16 cmd, const QByteArray& data);
  void sendCommLog(AbstractComm::LogLevel level,
                   quint16 errCmdInt,
                   const QString& msgLog);

private Q_SLOTS:
  void slotSysCmd(const quint16 cmd, const QByteArray& data);
  void slotResponse(const quint16 cmd, const QByteArray& data);
  void slotCommLog(AbstractComm::LogLevel level,
                   const QByteArray& errCmd,
                   const QString& msgStr);

private:
  Serial m_serial;
  CommState m_commState = XComm::COMM_IDLE;
  MotorState m_motorState = XComm::MOTOR_STOP;
  CommStats m_commStats;
  DriverDataType::RunMode currentRunMode = DriverDataType::MODE0;
};
#endif // XCOMM_H

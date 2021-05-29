/*
 * @Author: whj
 * @Date: 2021-02-01 11:58:59
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:05:05
 * @Description: file content
 */

#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include "../../BaseDataType/bitconverter.h"
#include "../../utilities/CRC.h"
#include "../abstractport.h"

#include <QMutex>
#include <QPair>
#include <QTime>
#include <QtSerialPort>

class Serial : public AbstractPort
{
  Q_OBJECT

public:
  struct SerialConfig
  {
    QString m_portNum;
    QSerialPort::BaudRate m_baudRate;
    int m_waitTimeout;

    SerialConfig()
      : m_portNum(QStringLiteral())
      , m_baudRate(QSerialPort::UnknownBaud)
      , m_waitTimeout(1000)
    {}
  };

public:
  explicit Serial(QObject* parent = nullptr);
  ~Serial();

  void transaction(const quint16 cmd, const QByteArray& data) override;
  void closePort() override;
  void openPort() override;
  bool isBusy() override;
  quint64 getTotalTimeElapse() override;
  void clearTotalTimeElapse() override;
  QString getPortType() override { return "Serial"; }
  void configSerial(const SerialConfig& config) { m_config = config; }
  const QString& getPortNum() const override { return m_config.m_portNum; }

private:
  void run() override;
  QByteArray constructData(const quint16 cmd, const QByteArray& cmdDat);
  typedef QPair<QByteArray, QByteArray> PaserDataType; //简化一下
  PaserDataType parseData(const QByteArray& rxdBuf, const QByteArray& reqCmd);

private:
  //帧头尾
  const char* frameHead = "{}"; // "0x7B 0x7D" head
  const char* frameTail = "[]"; // "0x5B 0x5D" tail
  //最大重发次数
  Q_CONSTEXPR static int maxRetry = 3;
  // 接收(rxd)总线空闲等待时间
  //如果收到数据不完整考虑增大该值
  Q_CONSTEXPR static int checkRxdIdleTime = 20; // 20ms
  //轮询request queue的时间间隔，增大间隔可减小CPU使用率
  Q_CONSTEXPR static int checkQueueInterval = 15; // 15ms

  QQueue<QByteArray> m_requestQueue;
  SerialConfig m_config;
  QMutex m_mutex;
  bool m_quit = true;
  bool m_isBusy = false;
  quint64 m_totalTimeElapse = 0;
};

#endif // SERIALCOMM_H

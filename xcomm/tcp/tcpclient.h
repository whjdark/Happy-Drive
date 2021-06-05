/*
 * @Author: whj
 * @Date: 2021-05-29 18:23:53
 * @LastEditors: whj
 * @LastEditTime: 2021-05-29 18:24:12
 * @Description: file content
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QHostAddress>
#include <QHostInfo>
#include <QTcpSocket>

#include "../../BaseDataType/driverdatatype.h"
#include "../abstractport.h"

// tcp client temporalily not need multi-thread

class TcpClient : public AbstractPort
{
  Q_OBJECT

public:
  struct TCPConfig
  {
    QString m_hostIP;
    int m_portNum;
    int m_waitTimeout;

    TCPConfig()
      : m_hostIP(QStringLiteral())
      , m_portNum(-1)
      , m_waitTimeout(3000)
    {}
  };

public:
  explicit TcpClient(QObject* parent = nullptr);
  ~TcpClient();

  void transaction(const quint16 cmd, const QByteArray& data) override;
  void closePort() override;
  void openPort() override;
  bool isBusy() override { return false; };
  quint64 getTotalTimeElapse() override { return 0; };
  void clearTotalTimeElapse() override { return; };
  AbstractPort::PortType getPortType() const override;
  QString getPortNum() const override;
  void configClient(const TCPConfig& config) { m_config = config; }

private:
  QHostAddress hostAddress() const;
  static QString localHost();

private Q_SLOTS:
  void slotReadData();
  void slotOnError();

private:
  QTcpSocket* m_tcpClient = nullptr;
  TCPConfig m_config;
  quint64 m_totalTimeElapse = 0;
};

#endif // TCPCLIENT_H

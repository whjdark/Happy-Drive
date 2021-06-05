/*
 * @Author: whj
 * @Date: 2021-02-01 11:58:59
 * @LastEditors: whj
 * @LastEditTime: 2021-05-29 18:26:11
 * @Description: file content
 */

#include "tcpclient.h"

TcpClient::TcpClient(QObject* parent)
  : AbstractPort(parent)
{}

TcpClient::~TcpClient() = default;

void
TcpClient::transaction(const quint16 cmd, const QByteArray& data)
{
  // cmd & data is designed for serialport communication
  // TCP no need cmd & data
  Q_UNUSED(cmd);
  // host & client is request & response
  m_tcpClient->write(data);
}

void
TcpClient::openPort()
{
  m_tcpClient = new QTcpSocket(this);
  QHostAddress hostAddr = hostAddress();
  if (hostAddr.isNull() || m_config.m_portNum == -1) {
    Q_EMIT signalLog(MSG_ERROR,
                     QByteArray(),
                     tr("[ERROR]Empty IP address（%1） or port number（%2）.")
                       .arg(m_config.m_hostIP)
                       .arg(m_config.m_portNum));
    return;
  }
  m_tcpClient->connectToHost(hostAddr, m_config.m_portNum);
  if (!m_tcpClient->waitForConnected(m_config.m_waitTimeout)) {
    Q_EMIT signalLog(
      MSG_ERROR,
      QByteArray(),
      tr("[ERROR]Can not connect to server!\n"
         "Please check the network, IP address（%1）and port number（%2）.")
        .arg(hostAddr.toString())
        .arg(m_config.m_portNum));
    return;
  }
  // successfully connect to Host, bind signals & slots
  connect(m_tcpClient, &QTcpSocket::readyRead, this, &TcpClient::slotReadData);
  auto errorSignal = QOverload<QTcpSocket::SocketError>::of(&QTcpSocket::error);
  connect(m_tcpClient, errorSignal, this, &TcpClient::slotOnError);
  // info user , 0x1001 means successfully connect to host
  Q_EMIT signalResponse(0x1001, QByteArray());
}

auto
TcpClient::getPortType() const -> AbstractPort::PortType
{
  return AbstractPort::TCPClient;
}

auto
TcpClient::getPortNum() const -> QString
{
  return m_config.m_hostIP + QStringLiteral(":") +
         QString::number(m_config.m_portNum);
}

void
TcpClient::closePort()
{
  m_tcpClient->close();
  m_tcpClient->deleteLater();
  m_tcpClient = nullptr;
}

void
TcpClient::slotReadData()
{
  // cmd is designed for serialport communication
  // 0x0000 means receive EtherCAT server data
  Q_EMIT signalResponse(0x0000, m_tcpClient->readAll());
}

void
TcpClient::slotOnError()
{
  Q_EMIT signalLog(MSG_ERROR,
                   QByteArray(),
                   tr("[ERROR]The remote host closed the connection.\n"
                      "IP address（%1）and port number（%2）.")
                     .arg(m_config.m_hostIP)
                     .arg(m_config.m_portNum));
}

auto
TcpClient::hostAddress() const -> QHostAddress
{
  QString ip = m_config.m_hostIP;
  QString addr = ip == QStringLiteral("localhost") ? localHost() : ip;
  return QHostAddress(addr);
}

auto
TcpClient::localHost() -> QString
{
  QString ip;
  QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
  if (info.addresses().isEmpty()) {
    return QString();
  }
  Q_FOREACH (const auto& addr, info.addresses()) {
    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
      // only use IPv4Protocol
      ip = addr.toString();
      break;
    }
  }
  return ip;
}

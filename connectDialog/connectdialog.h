/*
 * @Author: whj
 * @Date: 2021-01-31 20:54:20
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:03:35
 * @Description: file content
 */

#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "../xcomm/xcomm.h"

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ConnectDialog(QWidget* parent = nullptr);
  ~ConnectDialog();

  const Serial::SerialConfig& getSerialConfig() const { return m_serialConfig; }
  const TcpClient::TCPConfig& getTcpConfig() const { return m_tcpConfig; }
  AbstractPort::PortType getPortType() const;
  bool isConnect() const { return m_isConnect; }

private Q_SLOTS:
  void on_setButton_clicked();
  void on_detectButton_clicked();
  void on_cancelButton_clicked();
  void on_defaultButton_clicked();
  void on_detailButton_clicked();
  void on_commPortComboBox_currentIndexChanged(int index);

private:
  void initBaudBox();
  void detectPorts();
  void installReg();
  void showTcpClientDetail();
  void showSerialDetail();
  bool configTcpClient();
  bool configSerial();

private:
  //可用波特率 use c++11 initializer_list
  const QStringList baudList{
    "4800",   "9600",   "19200",  "38400",   "57600",
    "115200", "128000", "230400", "256000",  "460800",
    "500000", "576000", "921600", "1000000", "1152000"
  };
  Ui::ConnectDialog* ui;
  Serial::SerialConfig m_serialConfig;
  TcpClient::TCPConfig m_tcpConfig;
  bool m_isConnect = false;
};

#endif // CONNECTDIALOG_H

/*
 * @Author: whj
 * @Date: 2021-01-31 20:54:20
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:03:28
 * @Description: file content
 */

#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::ConnectDialog)
{
  ui->setupUi(this);

  // 模态窗口
  setWindowModality(Qt::ApplicationModal);
  //将窗口的属性设置为无边框, 不显示问号,
  setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint |
                 Qt::FramelessWindowHint);
  //侦测可用串口
  detectPorts();
  //初始化波特率box
  initBaudBox();
}

ConnectDialog::~ConnectDialog()
{
  delete ui;
}

void
ConnectDialog::initBaudBox()
{
  //波特率默认设置
  ui->baudComboBox->addItems(baudList);
  ui->baudComboBox->setCurrentIndex(5);
}

void
ConnectDialog::detectPorts()
{
  // clear portComboBox before append content
  ui->portComboBox->clear();
  //侦测可用串口
  Q_FOREACH (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
    QSerialPort serialPort;
    serialPort.setPort(info);
    if (serialPort.open(QIODevice::ReadWrite)) {
      ui->portComboBox->addItem(serialPort.portName());
      serialPort.clear();
      serialPort.close();
    }
  }
}

void
ConnectDialog::on_setButton_clicked()
{
  if (QSerialPortInfo::availablePorts().isEmpty()) {
    QMessageBox::warning(this,
                         QStringLiteral("串口无法打开"),
                         QStringLiteral("串口无法打开或被占用,"
                                        "请重试."));
    return;
  }
  m_config.m_portName = ui->portComboBox->currentText();
  if (m_config.m_portName.isEmpty()) {
    QMessageBox::warning(this,
                         QStringLiteral("未选择串口"),
                         QStringLiteral("未选择串口,"
                                        "请选择串口."));
    return;
  }
  switch (ui->baudComboBox->currentIndex()) {
    case 0:
      m_config.m_baudRate = QSerialPort::Baud4800;
      break;
    case 1:
      m_config.m_baudRate = QSerialPort::Baud9600;
      break;
    case 2:
      m_config.m_baudRate = QSerialPort::Baud19200;
      break;
    case 3:
      m_config.m_baudRate = QSerialPort::Baud38400;
      break;
    case 4:
      m_config.m_baudRate = QSerialPort::Baud57600;
      break;
    case 5:
      m_config.m_baudRate = QSerialPort::Baud115200;
      break;
    case 6:
      m_config.m_baudRate = QSerialPort::Baud128000;
      break;
    case 7:
      m_config.m_baudRate = QSerialPort::Baud230400;
      break;
    case 8:
      m_config.m_baudRate = QSerialPort::Baud256000;
      break;
    case 9:
      m_config.m_baudRate = QSerialPort::Baud460800;
      break;
    case 10:
      m_config.m_baudRate = QSerialPort::Baud500000;
      break;
    case 11:
      m_config.m_baudRate = QSerialPort::Baud576000;
      break;
    case 12:
      m_config.m_baudRate = QSerialPort::Baud921600;
      break;
    case 13:
      m_config.m_baudRate = QSerialPort::Baud1000000;
      break;
    case 14:
      m_config.m_baudRate = QSerialPort::Baud1152000;
      break;
    default:
      m_config.m_baudRate = QSerialPort::UnknownBaud;
      break;
  }
  m_config.m_waitTimeout = ui->waitResponsespinBox->value();
  //进行连接
  m_isConnect = true;
  this->close();
}

void
ConnectDialog::on_detectButton_clicked()
{
  detectPorts();
}

void
ConnectDialog::on_cancelButton_clicked()
{
  //不连接
  m_isConnect = false;
  close();
}

void
ConnectDialog::on_defaultButton_clicked()
{
  ui->portComboBox->setCurrentIndex(0);
  ui->baudComboBox->setCurrentIndex(5);
  ui->waitResponsespinBox->setValue(1000);
}

void
ConnectDialog::on_detailButton_clicked()
{
  const QString blankString(QStringLiteral("N/A"));

  QStringList list;
  QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
  list << tr("Total numbers of ports: %1").arg(serialPortInfos.count());

  Q_FOREACH (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
    QString portName = info.portName();
    QString description = info.description();
    QString manufacturer = info.manufacturer();
    QString serialNumber = info.serialNumber();
    list << "portName: " % portName
         << "description: " %
              (!description.isEmpty() ? description : blankString)
         << "manufacturer: " %
              (!manufacturer.isEmpty() ? manufacturer : blankString)
         << "serialNumber: " %
              (!serialNumber.isEmpty() ? serialNumber : blankString)
         << "systemLocation: " % info.systemLocation()
         << "vendorIdentifier: " %
              (info.vendorIdentifier() != 0U
                 ? QString::number(info.vendorIdentifier(), 16)
                 : blankString)
         << "productIdentifier: " %
              (info.productIdentifier() != 0U
                 ? QString::number(info.productIdentifier(), 16)
                 : blankString)
         << QStringLiteral("\n");
  }
  QMessageBox::information(
    this, QStringLiteral("PORT INFO"), list.join(QStringLiteral("\n")));
}

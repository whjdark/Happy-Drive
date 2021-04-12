/*
 * @Author: whj
 * @Date: 2021-02-01 11:58:59
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:06:03
 * @Description: file content
 */

#include "serialcomm.h"

#include <QSerialPort>
#include <QTime>

Serial::Serial(QObject* parent)
  : QThread(parent)
{
  //不同线程之间通过信号和槽来传递自定义数据类型,
  //需添加注册自定义类SerialMsgLevel类型
  qRegisterMetaType<Serial::LogLevel>("SerialThread::SerialMsgLevel");
}

Serial::~Serial()
{
  // stop loop
  m_mutex.lock();
  m_quit = true;
  m_mutex.unlock();
  // quit thread
  requestInterruption();
  quit();
  wait();
}

auto
Serial::constructData(const quint16 cmd, const QByteArray& cmdDat) -> QByteArray
{
  QByteArray frame;
  // reserve array space to avoid memory reallocate
  int frameLen = static_cast<int>(sizeof(cmd) + cmdDat.size() + 8);
  frame.reserve(frameLen);
  // add header
  frame.append(frameHead);
  // add length of cmdData (need<= 65536 bytes)
  auto cmdDataLen = static_cast<quint16>(cmdDat.size() + sizeof(cmd));
  frame.append(BitConverter::num2Ba(cmdDataLen));
  // add cmd
  frame.append(BitConverter::num2Ba(cmd));
  // add cmd data
  frame.append(cmdDat);
  // add CRC16 Modbus
  quint16 crc = CRCPP::getCRC16MB(&cmd, sizeof(cmd));
  if (!cmdDat.isEmpty()) {
    crc = CRCPP::getCRC16MB(cmdDat.data(), cmdDat.size(), crc);
  }
  frame.append(BitConverter::num2Ba(crc));
  // add tail
  frame.append(frameTail);
  return frame;
}

auto
Serial::parseData(const QByteArray& rxdBuf, const QByteArray& reqCmd)
  -> PaserDataType
{
  // seperate data from a whole frame
  if (rxdBuf.isEmpty()) {
    Q_EMIT commLog(
      MSG_WARNING, reqCmd, QStringLiteral("[WARNING]received data is empty"));
    return qMakePair(QByteArray(), QByteArray());
  }
  int headInd = -1;
  while (true) {
    //从下个位置开始寻找head和tail
    headInd++;
    // find a complete frame
    //从上一次的head开始寻找
    headInd = rxdBuf.indexOf(frameHead, headInd);
    //从head开始寻找配对的tail
    int tailInd = rxdBuf.indexOf(frameTail, headInd);
    if (headInd == -1 || tailInd == -1) {
      // no valid data found
      Q_EMIT commLog(
        MSG_WARNING, reqCmd, QStringLiteral("[WARNING]no valid data found"));
      // have problem, return empty
      return qMakePair(QByteArray(), QByteArray());
    }
    // check data length
    int recvLen = BitConverter::ba2Int(rxdBuf.mid(headInd + 2, 2));
    if ((tailInd - headInd) - 6 != recvLen) {
      Q_EMIT commLog(
        MSG_INFO, reqCmd, QStringLiteral("[INFO]cmd data length error"));
      continue;
    }
    // seperate cmd
    QByteArray rxdCmd = rxdBuf.mid(headInd + 4, 2);
    // requestCmd == recv cmd?
    if (reqCmd != rxdCmd) {
      Q_EMIT commLog(
        MSG_INFO, reqCmd, QStringLiteral("[INFO]request cmd != recv cmd"));
      continue;
    }
    // when data length >2 have cmd data
    // if no cmd data, received CRC will be 0xFFFF holdplace
    // seperate cmd data
    QByteArray rxdDat;
    if (recvLen > 2) {
      rxdDat = rxdBuf.mid(headInd + 6, recvLen - 2);
      // CRC check cmd data have problem temporarily cancelled
      //      quint16 crc = CRCPP::getCRC16MB(rxdDat.data(), rxdDat.size());
      //      quint16 recvCRC =
      //        static_cast<quint16>(BitConverter::ba2Int(rxdBuf.mid(tailInd -
      //        2, 2)));
      //      if (crc != recvCRC) {
      //        Q_EMIT commLog(
      //          MSG_INFO, reqCmd, QStringLiteral("[INFO]CRC check error"));
      //        continue;
      //      }
    }
    // no problem return cmd & data
    // extract one frame, others wil be ignored
    return qMakePair(rxdCmd, rxdDat);
  }
}

void
Serial::transaction(const quint16 cmd, const QByteArray& data)
{
  const QMutexLocker locker(&m_mutex);
  //命令加入队列
  m_requestQueue.enqueue(constructData(cmd, data));
}

void
Serial::run()
{
  qDebug() << "serial threadID : " << QThread::currentThreadId();

  // read serial config
  m_mutex.lock();
  QString currentPortName = m_config.m_portName;
  QSerialPort::BaudRate currentBaud = m_config.m_baudRate;
  int currentWaitTimeout = m_config.m_waitTimeout;
  m_mutex.unlock();
  // check serial is available
  QSerialPort serial;
  if (currentPortName.isEmpty()) {
    Q_EMIT commLog(
      MSG_ERROR, QByteArray(), QStringLiteral("[ERROR]No port name specified"));
    qDebug() << QThread::currentThreadId() << " quit";
    return;
  }
  serial.close();
  serial.setPortName(currentPortName);
  serial.setBaudRate(currentBaud);
  // open serial
  if (!serial.open(QIODevice::ReadWrite)) {
    Q_EMIT commLog(MSG_ERROR,
                   QByteArray(),
                   tr("[ERROR]Can't open %1, error code %2")
                     .arg(m_config.m_portName)
                     .arg(serial.error()));
    qDebug() << QThread::currentThreadId() << " quit";
    return;
  }
  // thread while
  while (!m_quit) { // serial loop quit
    //轮询队列，看看是否有新命令
    m_mutex.lock();
    bool isEmpty = m_requestQueue.isEmpty();
    m_mutex.unlock();
    if (isEmpty) { //没有新命令
      // sleep一下降低CPU使用率
      msleep(checkQueueEmptyInterval);
      continue;
    }
    //有新命令，通讯忙
    m_mutex.lock();
    QByteArray request = m_requestQueue.dequeue();
    QByteArray reqCmd = request.mid(4, 2);
    m_isBusy = true;
    m_mutex.unlock();
    // write request
    int retryCnt = 0;
    QTime qTime;
    qTime.start();
    while (!m_quit) { // serial loop quit
      retryCnt++;
      if (retryCnt > maxRetry) { // resend
        Q_EMIT commLog(
          MSG_ERROR, reqCmd, QStringLiteral("[ERROR]retry count > 3"));
        break;
      }
      // write request
      serial.write(request);
      // wait for write
      if (!serial.waitForBytesWritten(currentWaitTimeout)) {
        Q_EMIT commLog(MSG_WARNING,
                       reqCmd,
                       QStringLiteral("[WARNING]Wait write response timeout"));
        // retry
        continue;
      }
      // Discards all characters from the output or input buffer
      //防止粘包？
      serial.clear();
      if (!serial.waitForReadyRead(currentWaitTimeout)) {
        Q_EMIT commLog(MSG_WARNING,
                       reqCmd,
                       QStringLiteral("[WARNING]Wait read response timeout"));
        continue;
      }
      // read response
      QByteArray rxdBuf = serial.readAll();
      while (serial.waitForReadyRead(checkRxdIdleTime)) {
        //如果x ms没收到数据则认为本次通讯结束
        rxdBuf += serial.readAll();
      }
      // check received data
      PaserDataType rxdPair = parseData(rxdBuf, reqCmd);
      // is correct data?(rxd cmd is not empty)
      if (rxdPair.first.isEmpty()) {
        // retry
        continue;
      }
      // success
      auto int16Cmd = static_cast<quint16>(BitConverter::ba2Int(rxdPair.first));
      Q_EMIT response(int16Cmd, rxdPair.second);
      break; // go to sleep
    }
    m_totalTimeElapse += qTime.elapsed();
    // finish ,not in busy
    m_mutex.lock();
    m_isBusy = false;
    m_mutex.unlock();
  }
  qDebug() << QThread::currentThreadId() << " quit";
}

auto
Serial::isBusy() -> bool
{
  const QMutexLocker locker(&m_mutex);
  return m_isBusy;
}

auto
Serial::getTotalTimeElapse() -> quint64
{
  const QMutexLocker locker(&m_mutex);
  return m_totalTimeElapse;
}

void
Serial::clearTotalTimeElapse()
{
  const QMutexLocker locker(&m_mutex);
  m_totalTimeElapse = 0;
}

void
Serial::startComm()
{
  const QMutexLocker locker(&m_mutex);
  // start loop
  m_quit = false;
  start();
}

void
Serial::closeComm()
{
  const QMutexLocker locker(&m_mutex);
  // stop loop
  m_quit = true;
  m_requestQueue.clear();
}

#ifndef ABSTRACTCOMM_H
#define ABSTRACTCOMM_H

#include <QMetaType>
#include <QThread>

class AbstractComm : public QThread
{
public:
  enum LogLevel
  {
    MSG_OK = 0,      // level 0 = ok;
    MSG_INFO = 1,    // level 1 = INFO
    MSG_WARNING = 2, //  level 2 = WARNING
    MSG_ERROR = 3,   // level 3 = ERROR
  };
  Q_ENUM(LogLevel)

public:
  explicit AbstractComm(QObject* parent = nullptr)
    : QThread(parent)
  {
    //不同线程之间通过信号和槽来传递自定义数据类型,
    //需添加注册自定义类SerialMsgLevel类型
    qRegisterMetaType<LogLevel>("LogLevel");
  };
  ~AbstractComm() = default;

  virtual void transaction(const quint16 cmd, const QByteArray& data) = 0;
  virtual void closeComm() = 0;
  virtual void startComm() = 0;
  virtual bool isBusy() = 0;
  virtual quint64 getTotalTimeElapse() = 0;
  virtual void clearTotalTimeElapse() = 0;
  virtual QString type() = 0;

Q_SIGNALS:
  void response(const quint16 cmd, const QByteArray& data);
  void commLog(LogLevel level, const QByteArray& errCmd, const QString& msgStr);
};

#endif // ABSTRACTCOMM_H

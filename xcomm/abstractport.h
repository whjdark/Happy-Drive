#ifndef ABSTRACTPORT_H
#define ABSTRACTPORT_H

#include <QMetaType>
#include <QThread>

class AbstractPort : public QThread
{
  Q_OBJECT

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
  explicit AbstractPort(QObject* parent = nullptr)
    : QThread(parent)
  {
    //不同线程之间通过信号和槽来传递自定义数据类型,
    //需添加注册自定义类SerialMsgLevel类型
    qRegisterMetaType<LogLevel>("LogLevel");
  };
  ~AbstractPort() = default;

  virtual void transaction(const quint16 cmd, const QByteArray& data) = 0;
  virtual void closePort() = 0;
  virtual void openPort() = 0;
  virtual bool isBusy() = 0;
  virtual quint64 getTotalTimeElapse() = 0;
  virtual void clearTotalTimeElapse() = 0;
  virtual const QString& getPortNum() const = 0;
  virtual QString getPortType() = 0;

Q_SIGNALS:
  void signalResponse(const quint16 cmd, const QByteArray& data);
  // level 0 = ok; level 1 = INFO; level 2 = WARNING; level 3 = ERROR
  void signalLog(LogLevel level,
                 const QByteArray& errCmd,
                 const QString& msgStr);
};

#endif // ABSTRACTPORT_H

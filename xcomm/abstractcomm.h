#ifndef ABSTRACTCOMM_H
#define ABSTRACTCOMM_H

class AbstractComm
{
public:
  enum LogLevel
  {
    MSG_OK = 0,
    MSG_INFO = 1,
    MSG_WARNING = 2,
    MSG_ERROR = 3,
  };

public:
  virtual void transaction(const quint16 cmd, const QByteArray& data) = 0;
  virtual void closeComm() = 0;
  virtual void startComm() = 0;
  virtual bool isBusy() = 0;
  virtual quint64 getTotalTimeElapse() = 0;
  virtual void clearTotalTimeElapse() = 0;
  virtual QString type() = 0;

Q_SIGNALS:
  void response(const quint16 cmd, const QByteArray& data);
  // level 0 = ok; level 1 = INFO; level 2 = WARNING; level 3 = ERROR
  void commLog(LogLevel level, const QByteArray& errCmd, const QString& msgStr);
};

#endif // ABSTRACTCOMM_H

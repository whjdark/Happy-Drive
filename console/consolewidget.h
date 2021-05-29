#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QMessageBox>
#include <QWidget>

#include "../xcomm/xcomm.h"

namespace Ui {
class ConsoleWidget;
}

class ConsoleWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ConsoleWidget(QWidget* parent = nullptr, XComm* xcomm = nullptr);
  ~ConsoleWidget();

private Q_SLOTS:
  void slotProcessCommLog(AbstractPort::LogLevel level,
                          quint16 Cmd,
                          const QString& msgLog);
  void slotUpdateStats();
  void slotShowCmd(const quint16 cmd, const QByteArray& data);

  void on_sendButton_clicked();
  void on_clearLogButton_clicked();
  void on_clearCmdPushButton_clicked();
  void on_clearRecvPushButton_clicked();
  void on_clearCounterPushButton_clicked();

  void on_requestLcdNumber_overflow();
  void on_avgTimeLcdNumber_overflow();
  void on_warnLcdNumber_overflow();
  void on_errorLcdNumber_overflow();
  void on_txdLcdNumber_overflow();
  void on_rxdLcdNumber_overflow();

  void on_showRecvCheckBox_stateChanged(int state);

private:
  void initTextEditor();
  void initLcd();
  void initConnections();

private:
  Q_CONSTEXPR static int maxEditorLine = 5000;
  Q_CONSTEXPR static int updateCommStatsInterval = 1000;

  Ui::ConsoleWidget* ui;
  XComm* m_xcomm;

  QTimer* m_updateStatsTimer;
};

#endif // CONSOLEWIDGET_H

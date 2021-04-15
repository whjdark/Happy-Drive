#ifndef SWEEPINGWINDOW_H
#define SWEEPINGWINDOW_H

#include <QMainWindow>

#include "../../amplitudeAndPhaseGraph/amandphgraph.h"
#include "../../xcomm/xcomm.h"
#include "../../BaseDataType/driverdatatype.h"
#include "../../runconfig/runconfigdialog.h"

namespace Ui {
class SweepingWindow;
}

class SweepingWindow : public QMainWindow
{
  Q_OBJECT

private:
  using DataVector = QVector<QCPGraphData>;

public:
  explicit SweepingWindow(QWidget* parent = nullptr, XComm* xcomm = nullptr);
  ~SweepingWindow();

  void showBode(const DataVector& ampitude, const DataVector& phase);
  void showBode(const QByteArray& data);

private Q_SLOTS:
  void slotProccessCmd(const quint16 cmd, const QByteArray& data);

  void slotExportData();
  void slotSaveImage();
  void slotSwitchTracer();

  void on_startButton_clicked();
  void on_writeButton_clicked();

  private:
  void initBode();
  void initToolBar();

private:
  Ui::SweepingWindow* ui;
  XComm* m_xcomm;
};

#endif // SWEEPINGWINDOW_H

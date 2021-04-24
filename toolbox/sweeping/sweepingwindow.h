#ifndef SWEEPINGWINDOW_H
#define SWEEPINGWINDOW_H

#include <QMainWindow>

#include "../../BaseDataType/driverdatatype.h"
#include "../../amplitudeAndPhaseGraph/amandphgraph.h"
#include "../../runconfig/runconfigdialog.h"
#include "../../xcomm/xcomm.h"

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
  void parseAmplitudeData(const QByteArray& data);
  void parsePhaseData(const QByteArray& data);
  DriverDataType::RunMode mapSectionToRunMode(int section);

private:
  //扫频参数
  Q_CONSTEXPR static int sweepPoint = 512;
  Q_CONSTEXPR static int sweepRange = sweepPoint / 2; // half of samplePoint
  //扫频后请求数据延迟时间，因为扫频结束后并DSP并不能立即算出数据。
  Q_CONSTEXPR static int requestDataDelay = 1000; // 1000ms

  Ui::SweepingWindow* ui;
  XComm* m_xcomm;
  DriverDataType::SweepConfigType m_sweepConfig;
  DriverDataType::RunConfigType m_runConfig;
  DataVector m_amplitude;
  DataVector m_phase;
};

#endif // SWEEPINGWINDOW_H

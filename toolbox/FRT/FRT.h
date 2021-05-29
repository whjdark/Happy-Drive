#ifndef FRT_H
#define FRT_H

#include <QMainWindow>

#include "../../BaseDataType/driverdatatype.h"
#include "../../amplitudeAndPhaseGraph/amandphgraph.h"
#include "../../runconfig/runconfigdialog.h"
#include "../../xcomm/xcomm.h"

namespace Ui {
class FRT;
}

class FRT : public QMainWindow
{
  Q_OBJECT

private:
  using DataVector = QVector<QCPGraphData>;

public:
  explicit FRT(QWidget* parent = nullptr, XComm* xcomm = nullptr);
  ~FRT();

private Q_SLOTS:
  void slotProccessCmd(const quint16 cmd, const QByteArray& data);

  void slotExportData();
  void slotSaveImage();
  void slotSwitchTracer();

  void on_startButton_clicked();
  void on_writeButton_clicked();

  void on_readButton_clicked();

private:
  void initBode();
  void initToolBar();
  void parseAmplitudeData(const QByteArray& data);
  void parsePhaseData(const QByteArray& data);
  DriverDataType::RunMode mapSectionToRunMode(int section);

private:
  //扫频参数
  Q_CONSTEXPR static int defaultSampleFreq = 2000;
  Q_CONSTEXPR static int samplePts = 512;
  Q_CONSTEXPR static int freqPts = samplePts / 2; // half of samplePts

  Ui::FRT* ui;
  XComm* m_xcomm;
  DriverDataType::FRTConfigType m_FRTConfig;
  DriverDataType::RunConfigType m_runConfig;
  DataVector m_amplitude;
  DataVector m_phase;
};

#endif // FRT_H

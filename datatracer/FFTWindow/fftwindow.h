#ifndef FFTWINDOW_H
#define FFTWINDOW_H

#include <QMainWindow>

#include "../../amplitudeAndPhaseGraph/amandphgraph.h"

namespace Ui {
class FFTWindow;
}

class FFTWindow : public QMainWindow
{
  Q_OBJECT

private:
  using DataVector = QVector<QCPGraphData>;

public:
  explicit FFTWindow(QWidget* parent = nullptr);
  ~FFTWindow();

  int doFFT(const DataVector& data);

private Q_SLOTS:
  void slotExportData();
  void slotSaveImage();
  void slotSwitchTracer();

private:
  void initFFTWidget();
  void initToolBar();

private:
  Ui::FFTWindow* ui;
  AmAndPhGraph* m_FFTGraph;
};

#endif // FFTWINDOW_H

#ifndef FFTWIDGET_H
#define FFTWIDGET_H

#include <QMessageBox>
#include <QWidget>

#include "QCustomPlot/qcustomplot.h"
#include "utilities/fftw3.h"
#pragma comment(lib, "libfftw3-3.lib") // double版本
// #pragma comment(lib, "libfftw3f-3.lib")// float版本
// #pragma comment(lib, "libfftw3l-3.lib")// long double版本

namespace Ui {
class FFTWidget;
}

class FFTWidget : public QWidget
{
  Q_OBJECT

public:
  explicit FFTWidget(QWidget* parent = nullptr);
  ~FFTWidget();

  void setTracerVisible(bool isVisible);
  bool isTracerVisible() const { return m_isVisible; }
  void autoScaleAxis();
  void setTitle(const QString& title);
  void setAmplitudeTitle(const QString& xTitle, const QString& yTitle);
  void setPhaseTitle(const QString& xTitle, const QString& yTitle);
  int doFFTAndShowResult(const QVector<QCPGraphData>& data);

private:
  void initAmplitudeGraph();
  void initPhaseGraph();
  void initConnections();

private:
  Ui::FFTWidget* ui;
  bool m_isVisible = false;
  QVector<QCPGraphData> m_amplitude; //振幅谱
  QVector<QCPGraphData> m_phase;     //相位谱
};

#endif // FFTWIDGET_H

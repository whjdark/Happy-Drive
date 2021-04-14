#ifndef AMANDPHGRAPH_H
#define AMANDPHGRAPH_H

#include <QMessageBox>
#include <QWidget>

#include "QCustomPlot/qcustomplot.h"
#include "utilities/fftw3.h"
#pragma comment(lib, "libfftw3-3.lib") // double版本
// #pragma comment(lib, "libfftw3f-3.lib")// float版本
// #pragma comment(lib, "libfftw3l-3.lib")// long double版本

namespace Ui {
class AmAndPhGraph;
}

class AmAndPhGraph : public QWidget
{
  Q_OBJECT

private:
  using DataVector = QVector<QCPGraphData>;

public:
  explicit AmAndPhGraph(QWidget* parent = nullptr);
  ~AmAndPhGraph();

  void setTracerVisible(bool isVisible);
  bool isTracerVisible() const { return m_isVisible; }
  void autoScaleAxis();
  void setTitle(const QString& title);
  void setAmplitudeTitle(const QString& xTitle, const QString& yTitle);
  void setPhaseTitle(const QString& xTitle, const QString& yTitle);
  int doFFTAndShowResult(const DataVector& data);
  void setDataAndShowBode(const DataVector& amplitude, const DataVector& phase);

private:
  void initAmplitudeGraph();
  void initPhaseGraph();
  void initConnections();

private:
  Ui::AmAndPhGraph* ui;
  bool m_isVisible = false;
  DataVector m_amplitude; //振幅谱
  DataVector m_phase;     //相位谱
};

#endif // AMANDPHGRAPH_H

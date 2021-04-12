#ifndef FFTWINDOW_H
#define FFTWINDOW_H

#include <QMainWindow>

#include "../../FFTwidget/FFTwidget.h"

namespace Ui {
class FFTWindow;
}

class FFTWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit FFTWindow(QWidget* parent = nullptr);
  ~FFTWindow();

  int doFFT(const QVector<QCPGraphData>& data);

private:
  Ui::FFTWindow* ui;
  FFTWidget* m_FFTResult;
};

#endif // FFTWINDOW_H

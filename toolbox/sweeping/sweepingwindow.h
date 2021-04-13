#ifndef SWEEPINGWINDOW_H
#define SWEEPINGWINDOW_H

#include <QMainWindow>

namespace Ui {
class SweepingWindow;
}

class SweepingWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit SweepingWindow(QWidget *parent = nullptr);
  ~SweepingWindow();

private:
  Ui::SweepingWindow *ui;
};

#endif // SWEEPINGWINDOW_H

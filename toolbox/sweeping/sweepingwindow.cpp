#include "sweepingwindow.h"
#include "ui_sweepingwindow.h"

SweepingWindow::SweepingWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::SweepingWindow)
{
  ui->setupUi(this);
}

SweepingWindow::~SweepingWindow()
{
  delete ui;
}

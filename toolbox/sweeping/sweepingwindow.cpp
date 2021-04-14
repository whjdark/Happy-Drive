#include "sweepingwindow.h"
#include "ui_sweepingwindow.h"

SweepingWindow::SweepingWindow(QWidget* parent, XComm* xcomm)
  : QMainWindow(parent)
  , ui(new Ui::SweepingWindow)
  , m_xcomm(xcomm)
{
  ui->setupUi(this);
  initBode(); //  // FFTwidget can alse show bode
  initToolBar();
}

SweepingWindow::~SweepingWindow()
{
  delete ui;
}

void
SweepingWindow::initBode()
{
  // set main title
  ui->bode->setTitle(QStringLiteral("Bode Diagram"));
  // set Magnitude&tPhase graph xy title
  ui->bode->setAmplitudeTitle(QStringLiteral(),
                              QStringLiteral("Magnitude(dB)"));
  ui->bode->setPhaseTitle(QStringLiteral("frequency(rad/s)"),
                          QStringLiteral("Phase(deg)"));
  ui->bode->setTracerVisible(false); //默认关闭游标
}

void
SweepingWindow::initToolBar()
{
  //工具栏
  QToolBar* toolBar = this->addToolBar(QStringLiteral("工具栏"));
  toolBar->setIconSize(QSize(25, 25));
  toolBar->setMovable(false);
  QAction* exportDataAction = toolBar->addAction(QStringLiteral("保存数据"));
  QAction* saveImageAction = toolBar->addAction(QStringLiteral("保存图片"));
  QAction* autoScaleAction = toolBar->addAction(QStringLiteral("自动缩放"));
  QAction* switchTracerAction = toolBar->addAction(QStringLiteral("游标"));
  //工具栏动作
  using SW = SweepingWindow;
  //导出数据
  connect(exportDataAction, &QAction::triggered, this, &SW::slotExportData);
  //导出图片
  connect(saveImageAction, &QAction::triggered, this, &SW::slotSaveImage);
  //自动缩放
  connect(
    autoScaleAction, &QAction::triggered, ui->bode, &AmAndPhGraph::autoScaleAxis);
  //开启/关闭游标
  connect(switchTracerAction, &QAction::triggered, this, &SW::slotSwitchTracer);
}

void
SweepingWindow::slotExportData()
{}

void
SweepingWindow::slotSaveImage()
{
  QPixmap p = ui->bode->grab();
  QImage img = p.toImage();
  if (img.isNull()) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("截取图像失败"));
  }
  //保存文件对话框
  QDateTime dateTime(QDateTime::currentDateTime());
  QString format = QStringLiteral("yyyyMMdd-hhmmss");
  QString timeStr = dateTime.toString(format);
  QString fileName = QFileDialog::getSaveFileName(
    this,
    QStringLiteral("保存数据图像"),
    QStringLiteral("bode_") + timeStr + QStringLiteral(".png"),
    QStringLiteral("Images (*.png)"));
  if (fileName.isEmpty()) {
    return;
  }
  //保存文件
  if (img.save(fileName)) { // 打开/创建图像成功
    QMessageBox::information(
      this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
  } else { // 打开/创建图像失败
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("创建图像失败！"));
  }
}

void
SweepingWindow::slotSwitchTracer()
{
  if (ui->bode->isTracerVisible()) {
    ui->bode->setTracerVisible(false);
  } else {
    ui->bode->setTracerVisible(true);
  }
}

void
SweepingWindow::showBode(const DataVector& ampitude, const DataVector& phase)
{
  ui->bode->setDataAndShowBode(ampitude, phase);
}

void
SweepingWindow::on_pushButton_clicked()
{}

void
SweepingWindow::on_startButton_clicked()
{}

#include "fftwindow.h"
#include "ui_fftwindow.h"

FFTWindow::FFTWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::FFTWindow)
  , m_FFTGraph(new AmAndPhGraph(this))
{
  ui->setupUi(this);
  initFFTWidget();
  initToolBar();
}

FFTWindow::~FFTWindow()
{
  delete ui;
}

void
FFTWindow::initFFTWidget()
{
  setCentralWidget(m_FFTGraph);
  // set main title
  m_FFTGraph->setTitle(QStringLiteral("DFT result"));
  // set Amplitude&tPhase graph xy title
  m_FFTGraph->setAmplitudeTitle(QStringLiteral(), QStringLiteral("Amplitude"));
  m_FFTGraph->setPhaseTitle(QStringLiteral("frequency(Hz)"),
                            QStringLiteral("Phase(deg)"));
  m_FFTGraph->setTracerVisible(false); //默认关闭游标
}

void
FFTWindow::initToolBar()
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
  using FW = FFTWindow;
  //导出数据
  connect(exportDataAction, &QAction::triggered, this, &FW::slotExportData);
  //导出图片
  connect(saveImageAction, &QAction::triggered, this, &FW::slotSaveImage);
  //自动缩放
  connect(autoScaleAction,
          &QAction::triggered,
          m_FFTGraph,
          &AmAndPhGraph::autoScaleAxis);
  //开启/关闭游标
  connect(switchTracerAction, &QAction::triggered, this, &FW::slotSwitchTracer);
}

void
FFTWindow::slotExportData()
{}

void
FFTWindow::slotSaveImage()
{
  QPixmap p = m_FFTGraph->grab();
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
    QStringLiteral("FFT_") + timeStr + QStringLiteral(".png"),
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
FFTWindow::slotSwitchTracer()
{
  if (m_FFTGraph->isTracerVisible()) {
    m_FFTGraph->setTracerVisible(false);
  } else {
    m_FFTGraph->setTracerVisible(true);
  }
}

auto
FFTWindow::doFFT(const DataVector& data) -> int
{
  return m_FFTGraph->doFFTAndShowResult(data);
}

#include "fftwindow.h"
#include "ui_fftwindow.h"

FFTWindow::FFTWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::FFTWindow)
  , m_FFTResult(new FFTWidget(this))
{
  ui->setupUi(this);
  setCentralWidget(m_FFTResult);
  // set main title
  m_FFTResult->setTitle(QStringLiteral("DFT result"));
  // set Amplitude&tPhase graph xy title
  m_FFTResult->setAmplitudeTitle(QStringLiteral(), QStringLiteral("Amplitude"));
  m_FFTResult->setPhaseTitle(QStringLiteral("frequency(Hz)"),
                             QStringLiteral("Phase(deg)"));
  m_FFTResult->setTracerVisible(false); //默认关闭游标
  //工具栏
  QToolBar* toolBar = this->addToolBar(QStringLiteral("工具栏"));
  toolBar->setIconSize(QSize(25, 25));
  toolBar->setMovable(false);
  QAction* exportDataAction = toolBar->addAction(QStringLiteral("保存数据"));
  QAction* exportImageAction = toolBar->addAction(QStringLiteral("保存图片"));
  QAction* autoScaleAction = toolBar->addAction(QStringLiteral("自动缩放"));
  QAction* tracerAction = toolBar->addAction(QStringLiteral("游标"));
  //工具栏动作
  //导出数据
  connect(exportDataAction, &QAction::triggered, this, [=]() {
    // TBC
  });
  //导出图片
  connect(exportImageAction, &QAction::triggered, this, [=]() {
    QPixmap p = m_FFTResult->grab();
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
  });
  //自动缩放
  connect(autoScaleAction, &QAction::triggered, this, [=]() {
    m_FFTResult->autoScaleAxis();
  });
  //开启/关闭游标
  connect(tracerAction, &QAction::triggered, this, [=]() {
    if (m_FFTResult->isTracerVisible()) {
      m_FFTResult->setTracerVisible(false);
    } else {
      m_FFTResult->setTracerVisible(true);
    }
  });
}

FFTWindow::~FFTWindow()
{
  delete ui;
}

auto
FFTWindow::doFFT(const QVector<QCPGraphData>& data) -> int
{
  return m_FFTResult->doFFTAndShowResult(data);
}

#include "sweepingwindow.h"
#include "ui_sweepingwindow.h"

SweepingWindow::SweepingWindow(QWidget* parent, XComm* xcomm)
  : QMainWindow(parent)
  , ui(new Ui::SweepingWindow)
  , m_xcomm(xcomm)
{
  ui->setupUi(this);
  initBode();
  initToolBar();
  connect(m_xcomm,
          &XComm::toolboxSweepingCmd,
          this,
          &SweepingWindow::slotProccessCmd);
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
  connect(autoScaleAction,
          &QAction::triggered,
          ui->bode,
          &AmAndPhGraph::autoScaleAxis);
  //开启/关闭游标
  connect(switchTracerAction, &QAction::triggered, this, &SW::slotSwitchTracer);
}

void
SweepingWindow::slotProccessCmd(const quint16 cmd, const QByteArray& data)
{
  switch (cmd) {
    case XComm::TOOLBOX_SWEEPING_WRITE:
      QMessageBox::information(
        this, QStringLiteral("提示"), QStringLiteral("参数写入成功"));
      break;
    case XComm::TOOLBOX_SWEEPING_REQ_DATA:
      showBode(data);
      break;
    default:
      break;
  }
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
  if (img.save(fileName)) {
    // 打开/创建图像成功
    QMessageBox::information(
      this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
  } else {
    // 打开/创建图像失败
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
SweepingWindow::showBode(const QByteArray& data)
{}

void
SweepingWindow::on_startButton_clicked()
{
  RunConfigDialog runConfigDialog;
  if (m_xcomm->getConnectStatus() == XComm::COMM_IDLE) {
    // 未连接
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("未连接驱动器"));
    return;
  }
  if (m_xcomm->getMotorStatus() == XComm::MOTOR_RUN) {
    // if motor is running , no action
    QMessageBox::warning(this,
                         QStringLiteral("警告"),
                         QStringLiteral("电机已在运行，不要重复发送指令！"));
    return;
  }
  //运行参数配置对话框，提示当前运行模式
  QString currentRunMode = tr("Run Mode %1").arg(m_xcomm->getCurrentRunMode());
  runConfigDialog.setRunModeInfo(currentRunMode);
  if (runConfigDialog.exec() == QDialog::Rejected) {
    return;
  }
  //添加运行参数
  DriverDataType::RunConfigType runConfig = runConfigDialog.getRunConfig();
  // if motor is stop，start it
  m_xcomm->startMotor(runConfig);
}

void
SweepingWindow::on_writeButton_clicked()
{
  using namespace DriverDataType;
  SweepingConfigType sweepingConfig;
  // read sweeping config
  // some data should be scaled for transmiton
  sweepingConfig.data().m_targetLoop = ui->loopComboBox->currentIndex();
  sweepingConfig.data().m_ref = ui->refDoubleSpinBox->value() * scaleFactorIQ15;
  sweepingConfig.data().m_amplitude =
    ui->amDoubleSpinBox->value() * scaleFactorIQ15;
  sweepingConfig.data().m_minRange = ui->freqRangeMinSpinBox->value();
  sweepingConfig.data().m_maxRange = ui->freqRangeMaxSpinBox->value();
  sweepingConfig.data().m_sweepingStep =
    ui->sweepStepDoubleSpinBox->value() * scaleFactorIQ15;
  m_xcomm->command(XComm::TOOLBOX_SWEEPING_WRITE, sweepingConfig.toByteArray());
}

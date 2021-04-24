#include "sweepingwindow.h"
#include "ui_sweepingwindow.h"

SweepingWindow::SweepingWindow(QWidget* parent, XComm* xcomm)
  : QMainWindow(parent)
  , ui(new Ui::SweepingWindow)
  , m_xcomm(xcomm)
  , m_sweepConfig()
  , m_runConfig(2000, 256)
{
  ui->setupUi(this);
  initBode();
  initToolBar();
  using SW = SweepingWindow;
  connect(m_xcomm, &XComm::toolboxSweepingCmd, this, &SW::slotProccessCmd);
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
  using APG = AmAndPhGraph;
  //导出数据
  connect(exportDataAction, &QAction::triggered, this, &SW::slotExportData);
  //导出图片
  connect(saveImageAction, &QAction::triggered, this, &SW::slotSaveImage);
  //自动缩放
  connect(autoScaleAction, &QAction::triggered, ui->bode, &APG::autoScaleAxis);
  //开启/关闭游标
  connect(switchTracerAction, &QAction::triggered, this, &SW::slotSwitchTracer);
}

void
SweepingWindow::slotProccessCmd(const quint16 cmd, const QByteArray& data)
{
  switch (cmd) {
    case XComm::TOOLBOX_SWEEPING_WRITE:
      // log current run mode
      m_xcomm->logRunMode(
        static_cast<DriverDataType::RunMode>(m_sweepConfig.data().m_runMode));
      QMessageBox::information(
        this, QStringLiteral("提示"), QStringLiteral("参数写入成功"));
      break;
    case XComm::TOOLBOX_SWEEPING_REQ_AM:
      parseAmplitudeData(data);
      break;
    case XComm::TOOLBOX_SWEEPING_REQ_PH:
      parsePhaseData(data);
      break;
    default:
      break;
  }
}

void
SweepingWindow::slotExportData()
{
  // have data in graph ?
  if (m_amplitude.size() == 0 || m_phase.size() == 0) {
    // no data in graph
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("无数据"));
    return;
  }
  //保存文件对话框
  QDateTime dateTime(QDateTime::currentDateTime());
  QString format = QStringLiteral("yyyyMMdd-hhmmss");
  QString timeStr = dateTime.toString(format);
  QString fileName = QFileDialog::getSaveFileName(
    this,
    QStringLiteral("保存数据"),
    qApp->applicationDirPath() % QLatin1String("SweepBode_") % timeStr %
      QLatin1String(".csv"),
    QStringLiteral("Files (*.csv *.txt)"));
  if (fileName.isEmpty()) {
    return;
  }
  //保存文件
  QFile file(fileName);
  //以只写方式打开
  // Truncate清空文件
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("打开/创建文件失败！"));
    return;
  }
  //写入每一行数据到文件
  //如果am和ph数据长度不一致此处可能有段错误
  //频率|幅值|相位
  int i = 0;
  Q_FOREACH (const auto& pt, m_amplitude) {
    QTextStream out(&file);
    out << pt.key << ',' << pt.value << ',' << m_phase[i].value << "\n";
    i++;
  }
  file.close();
  QMessageBox::information(
    this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
}

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
SweepingWindow::parseAmplitudeData(const QByteArray& data)
{
  // also see TracerWidget::storeTransformedData
  using namespace BitConverter;
  using namespace DriverDataType;
  // clear stora before append new data
  m_amplitude.clear();
  m_amplitude.reserve(sweepRange);
  // update step is bytes of data
  int step = sizeof(amplitudeDataType);
  size_t totalByteLen = sweepRange * step;
  Q_ASSERT_X((size_t)data.size() == totalByteLen,
             "parseAmplitudeData",
             "recv data length is wrong");
  double transFormFactor = 1 / _IQ15;
  double f0 = m_runConfig.data().m_sampleFreq / sweepPoint; //分辨率
  for (size_t indexOfArray = 0, indexOfPoint = 0; indexOfArray < totalByteLen;
       indexOfArray += step, indexOfPoint++) {
    //将字节数据转成整型，并且对数据进行变换
    double am = ba2Int(data.mid(indexOfArray, step)) * transFormFactor;
    double mag = 20 * log10(am + eps); // 转换成Mag,加个小数防止0出现
    // x根据采样周期和采样点数变换到Hz
    double f = indexOfPoint * f0;
    double w = f * (2 * M_PI); //变换到rad/s
    m_amplitude.append(QCPGraphData(w, mag));
  }
  ui->bode->showAmplitude(m_amplitude);
}

void
SweepingWindow::parsePhaseData(const QByteArray& data)
{
  // also see TracerWidget::storeTransformedData
  using namespace BitConverter;
  using namespace DriverDataType;
  // clear stora before append new data
  m_phase.clear();
  m_phase.reserve(sweepRange);
  // update step is bytes of data
  int step = sizeof(phaseDataType);
  size_t totalByteLen = sweepRange * step;
  Q_ASSERT_X((size_t)data.size() == totalByteLen,
             "parsePhaseData",
             "recv data length is wrong");
  double transFormFactor = 1 / _IQ12;
  for (size_t indexOfArray = 0, indexOfPoint = 0; indexOfArray < totalByteLen;
       indexOfArray += step, indexOfPoint++) {
    //将字节数据转成整型，并且对数据进行变换
    double phase = ba2Int(data.mid(indexOfArray, step)) * transFormFactor;
    double phaseDeg = phase * 180 / M_PI; //化成deg(°)位
    //直接使用amplitude计算好的w，减少重复计算
    //如果am和ph数据长度不一致此处可能有段错误
    double w = m_amplitude[indexOfPoint].key;
    m_phase.append(QCPGraphData(w, phaseDeg));
  }
  ui->bode->showPhase(m_phase);
}

void
SweepingWindow::on_startButton_clicked()
{
  using namespace DriverDataType;
  RunConfigDialog runConfigDialog;
  if (m_xcomm->getConnectStatus() == XComm::COMM_IDLE) {
    // 未连接
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("未连接驱动器"));
    return;
  }
  if (m_xcomm->getMotorStatus() == XComm::MOTOR_RUN) {
    // if motor is running , no action
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("电机已在运行，请先停止！"));
    return;
  }
  DriverDataType::RunMode currRunMode = m_xcomm->getCurrentRunMode();
  if (currRunMode != DriverDataType::MODE_SWEEP_1 &&
      currRunMode != DriverDataType::MODE_SWEEP_2) {
    // not in sweeping modes
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("非扫频模式，请切换模式！"));
    return;
  }
  //运行参数配置对话框，提示当前运行模式
  runConfigDialog.setRunModeInfo(m_xcomm->getCurrentRunModeStr());
  if (runConfigDialog.exec() == QDialog::Rejected) {
    return;
  }
  //添加运行参数
  m_runConfig = runConfigDialog.getRunConfig();
  // if motor is stop，start it
  m_xcomm->startMotor(m_runConfig);
  // after 100ms ,check motor status
  QTimer::singleShot(2000, this, [=]() {
    if (m_xcomm->getMotorStatus() == XComm::MOTOR_RUN) {
      double sampleFreq = m_runConfig.data().m_sampleFreq;
      //计算扫频时间：扫描一个频率点的时间 * 扫频范围,
      double timeToRequestResult =
        ((double)sweepPoint / sampleFreq) * sweepRange * 1400;
      //加上一定时间等待DSP处理事务
      timeToRequestResult += requestDataDelay;
      timeToRequestResult = static_cast<int>(timeToRequestResult);
      //向DSP请求读取幅值、相位数据
      QTimer::singleShot(timeToRequestResult, this, [=]() {
        m_xcomm->stopMotor(); //先关闭电机
        //再读取数据
        m_xcomm->command(XComm::TOOLBOX_SWEEPING_REQ_AM, QByteArray());
        m_xcomm->command(XComm::TOOLBOX_SWEEPING_REQ_PH, QByteArray());
      });
    } else { // MOTOR_STOP
      QMessageBox::warning(
        this, QStringLiteral("警告"), QStringLiteral("启动失败"));
    }
  });
}

auto
SweepingWindow::mapSectionToRunMode(int section) -> DriverDataType::RunMode
{
  using namespace DriverDataType;
  RunMode runMode = MODE_SWEEP_1;
  switch (section) {
    case 0:
      //机械环节
      runMode = MODE_SWEEP_1;
      break;
    case 1:
      //电磁环节
      runMode = MODE_SWEEP_2;
      break;
    default:
      break;
  }
  return runMode;
}

void
SweepingWindow::on_writeButton_clicked()
{
  using namespace DriverDataType;
  // read sweeping config
  // some data should be scaled for transmition
  RunMode runMode = mapSectionToRunMode(ui->sectionComboBox->currentIndex());
  m_sweepConfig.data().m_runMode = runMode;
  m_sweepConfig.data().m_ref = ui->refDoubleSpinBox->value() * _IQ15;
  m_sweepConfig.data().m_amplitude = ui->amDoubleSpinBox->value() * _IQ15;
  // Range、step参数现在没用 TBC
  m_sweepConfig.data().m_minRange = ui->freqRangeMinSpinBox->value();
  m_sweepConfig.data().m_maxRange = ui->freqRangeMaxSpinBox->value();
  m_sweepConfig.data().m_sweepingStep =
    ui->sweepStepDoubleSpinBox->value() * _IQ15;
  m_xcomm->command(XComm::TOOLBOX_SWEEPING_WRITE, m_sweepConfig.toByteArray());
}

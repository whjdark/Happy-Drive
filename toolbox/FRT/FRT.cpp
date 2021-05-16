#include "FRT.h"
#include "ui_FRT.h"

FRT::FRT(QWidget* parent, XComm* xcomm)
  : QMainWindow(parent)
  , ui(new Ui::FRT)
  , m_xcomm(xcomm)
  , m_sweepConfig()
  , m_runConfig(2000, 256)
{
  ui->setupUi(this);
  initBode();
  initToolBar();
  connect(m_xcomm, &XComm::toolboxSweepingCmd, this, &FRT::slotProccessCmd);
}

FRT::~FRT()
{
  delete ui;
}

void
FRT::initBode()
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
FRT::initToolBar()
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
  using APG = AmAndPhGraph;
  //导出数据
  connect(exportDataAction, &QAction::triggered, this, &FRT::slotExportData);
  //导出图片
  connect(saveImageAction, &QAction::triggered, this, &FRT::slotSaveImage);
  //自动缩放
  connect(autoScaleAction, &QAction::triggered, ui->bode, &APG::autoScaleAxis);
  //开启/关闭游标
  connect(
    switchTracerAction, &QAction::triggered, this, &FRT::slotSwitchTracer);
}

void
FRT::slotProccessCmd(const quint16 cmd, const QByteArray& data)
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
FRT::slotExportData()
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
    out << pt.key << ',' << pt.value << ',' << m_phase.at(i).value << "\n";
    i++;
  }
  file.close();
  QMessageBox::information(
    this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
}

void
FRT::slotSaveImage()
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
    QStringLiteral("sweepBode_") + timeStr + QStringLiteral(".png"),
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
FRT::slotSwitchTracer()
{
  if (ui->bode->isTracerVisible()) {
    ui->bode->setTracerVisible(false);
  } else {
    ui->bode->setTracerVisible(true);
  }
}

void
FRT::parseAmplitudeData(const QByteArray& data)
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
  double f0 = (double)m_runConfig.data().m_sampleFreq / sweepPoint; //分辨率
  //第一个点为直流分量，应当跳过
  for (size_t indexOfArray = step, indexOfPoint = 1;
       indexOfArray < totalByteLen;
       indexOfArray += step, indexOfPoint++) {
    //将字节数据转成整型，并且对数据进行变换
    double am = ba2Int(data.mid(indexOfArray, step)) * transFormFactor;
    if (am == 0.0) {
      qDebug() << "apmlitude should ！= 0, but receive 0";
    }
    double mag = 20 * log10(am); // 转换成Mag,加个小数防止0出现
    // x根据采样周期和采样点数变换到Hz
    double f = indexOfPoint * f0;
    double w = f * (2 * M_PI); //变换到rad/s
    m_amplitude.append(QCPGraphData(w, mag));
  }
  ui->bode->showAmplitude(m_amplitude);
}

void
FRT::parsePhaseData(const QByteArray& data)
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
  //第一个点为直流分量，应当跳过
  for (size_t indexOfArray = step, indexOfPoint = 1;
       indexOfArray < totalByteLen;
       indexOfArray += step, indexOfPoint++) {
    //将字节数据转成整型，并且对数据进行变换
    double phase = ba2Int(data.mid(indexOfArray, step)) * transFormFactor;
    double phaseDeg = phase * 180 / M_PI; //化成deg(°)位
    // 计算幅值时算过w（角频率），直接使用
    // indexOfArray-1与m_amplitude下标相同
    double w = m_amplitude.at(indexOfPoint - 1).key;
    m_phase.append(QCPGraphData(w, phaseDeg));
  }
  ui->bode->showPhase(m_phase);
}

void
FRT::on_startButton_clicked()
{
  using namespace DriverDataType;
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
  RunConfigDialog runConfigDialog;
  runConfigDialog.setSampleFrequency(defaultSampleFreq);
  runConfigDialog.setRunModeInfo(m_xcomm->getCurrentRunModeStr());
  if (runConfigDialog.exec() == QDialog::Rejected) {
    return;
  }
  //添加运行参数
  m_runConfig = runConfigDialog.getRunConfig();
  // if motor is stop，start it
  m_xcomm->startMotor(m_runConfig);
}

auto
FRT::mapSectionToRunMode(int section) -> DriverDataType::RunMode
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
FRT::on_writeButton_clicked()
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

void
FRT::on_readButton_clicked()
{
  m_xcomm->command(XComm::TOOLBOX_SWEEPING_REQ_AM, QByteArray());
  m_xcomm->command(XComm::TOOLBOX_SWEEPING_REQ_PH, QByteArray());
}

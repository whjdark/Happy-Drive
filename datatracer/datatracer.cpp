/*
 * @Author: whj
 * @Date: 2021-03-02 14:35:16
 * @LastEditors: whj
 * @LastEditTime: 2021-04-09 22:01:07
 * @Description: file content
 */

#include "datatracer.h"

#include "ui_datatracer.h"

TracerWidget::TracerWidget(QWidget* parent, XComm* xcomm)
  : QWidget(parent)
  , ui(new Ui::DataTracer)
  , m_xcomm(xcomm)
  , m_controllerConfiger(new CONConfigQDialog(this, xcomm))
  , m_runConfiger(new RunConfigDialog(this))
  , m_statisticsWindow(new StatisticsWindow(this))
  , m_fftWindow(new FFTWindow(this))
  , m_updateStatsTimer(new QTimer(this))
{
  ui->setupUi(this);

  //初始化多通道设定
  initChart();
  initChannels();
  initPannel();
  initStatisticsWindow();
  initToolBox();
  initConnections();
  ui->customPlot->replot();
}

TracerWidget::~TracerWidget()
{
  delete ui;
}

void
TracerWidget::initChart()
{
  // chart setting
  // add title layout element:
  ui->customPlot->plotLayout()->insertRow(0);
  m_title = new QCPTextElement(ui->customPlot,
                               defautGraphTitle,
                               QFont(QStringLiteral("sans"), 17, QFont::Bold));
  ui->customPlot->plotLayout()->addElement(0, 0, m_title);
  //图例
  ui->customPlot->legend->setVisible(defaultLegend);
  QFont legendFont = font(); // start out with MainWindow's font..
  legendFont.setPointSize(10);
  ui->customPlot->legend->setFont(legendFont);
  ui->customPlot->legend->setBrush(QBrush(QColor(200, 200, 200, 230)));
  // by default, the legend is in the inset layout of the main axis rect.
  //默认图例左下
  ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(
    0, Qt::AlignBottom | Qt::AlignRight);
  // legend box shall not be selectable, only legend items
  ui->customPlot->legend->setSelectableParts(QCPLegend::spItems);
  //设置基本坐标轴（左侧Y轴和下方X轴）
  // 可拖动、可缩放、曲线可选、
  // legend可选、设置伸缩比例，使所有图例可见
  ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
                                  QCP::iSelectAxes | QCP::iSelectLegend |
                                  QCP::iSelectPlottables);
  //默认使用dark主题
  setDarkTheme();
  //拖动时关闭抗锯齿
  // more performance/responsiveness during dragging
  ui->customPlot->setNoAntialiasingOnDrag(false);
  // Defines plotting hints that control the quality and speed of plotting
  // speed of plotting axis labels will be cached as pixmaps
  ui->customPlot->setPlottingHint(QCP::phCacheLabels, true);
}

void
TracerWidget::initChannels()
{
  // 创建 x 个通道数据
  for (size_t ch = 0; ch < maxChannels; ch++) {
    // setup for graph: key axis left, value axis bottom
    ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
    // line color for graph
    ui->customPlot->graph(ch)->setPen(QPen(channelColors[ch]));
    // name for graph
    ui->customPlot->graph(ch)->setName(
      tr("CH%1-%2").arg(ch + 1).arg(channelList[0]));
  }
  // set axis labels:
  ui->customPlot->xAxis->setLabel(defaultXTitle);
  ui->customPlot->yAxis->setLabel(defaultYTitle);
  //坐标轴范围、刻度
  ui->customPlot->xAxis->setRange(defalutMinX, defalutMaxX);
  ui->customPlot->yAxis->setRange(defalutMinY, defalutMaxY);
  //可读性优于设置
  ui->customPlot->xAxis->ticker()->setTickStepStrategy(
    QCPAxisTicker::tssReadability);
  // 刻度线数量
  ui->customPlot->xAxis->ticker()->setTickCount(defalutXTick);
  ui->customPlot->yAxis->ticker()->setTickCount(defalutYTick);
  // openGL
  ui->customPlot->setOpenGl(defaultOpenGL);
  //坐标轴精度
  ui->customPlot->xAxis->setNumberPrecision(5);
  ui->customPlot->yAxis->setNumberPrecision(5);
  //坐标轴格式
  // g灵活的格式,b漂亮的指数形式，c乘号改成×
  ui->customPlot->xAxis->setNumberFormat("gbc");
  ui->customPlot->yAxis->setNumberFormat("gbc");
  //初始化游标,在此之前先初始化图像
  ui->customPlot->initGraphTracer();
  //游标，默认关闭
  ui->customPlot->setTracerVisible(defaultTracerVisible);
}

void
TracerWidget::initPannel()
{
  //坐标轴范围、刻度
  // x
  ui->xMaxDoubleSpinBox->setValue(defalutMaxX);
  ui->xMinDoubleSpinBox->setValue(defalutMinX);
  // y
  ui->yMaxDoubleSpinBox->setValue(defalutMaxY);
  ui->yMinDoubleSpinBox->setValue(defalutMinY);
  // 刻度线数量
  ui->xTickSpinBox->setValue(defalutXTick);
  ui->yTickSpinBox->setValue(defalutYTick);
  //图例
  ui->legendBox->setChecked(defaultLegend);
  //通道设定
  // checkbox在UI上默认设置为checked
  //在此处调整check状态作为默认设定
  //如果在此处设为false则会触发stateChange信号
  ui->ch1Box->setChecked(true);
  ui->ch2Box->setChecked(false);
  ui->ch3Box->setChecked(false);
  ui->ch4Box->setChecked(false);
  ui->ch5Box->setChecked(false);
  ui->ch6Box->setChecked(false);
  ui->ch7Box->setChecked(false);
  ui->ch8Box->setChecked(false);
  // openGL
  ui->openGLBox->setChecked(defaultOpenGL);
  // 默认使用dark主题
  ui->themeBox->setCurrentIndex(DarkTheme);
  //通道下拉框添加选项
  ui->ch1ComboBox->addItems(channelList);
  ui->ch2ComboBox->addItems(channelList);
  ui->ch3ComboBox->addItems(channelList);
  ui->ch4ComboBox->addItems(channelList);
  ui->ch5ComboBox->addItems(channelList);
  ui->ch6ComboBox->addItems(channelList);
  ui->ch7ComboBox->addItems(channelList);
  ui->ch8ComboBox->addItems(channelList);
  //默认不开启游标
  ui->tracerCheckBox->setChecked(defaultTracerVisible);
}

void
TracerWidget::initStatisticsWindow()
{
  //统计信息表格的row Index = graph Index = channel -1
  //初始化统计信息的行表头
  for (int ch = 0; ch < ui->customPlot->graphCount(); ch++) {
    //设置表头标题
    m_statisticsWindow->setRowHeaderText(
      ch, tr("CH%1-%2").arg(ch + 1).arg(channelList[0]));
    //设置每一行的文字颜色和对应的通道颜色相同
    m_statisticsWindow->setRowTextColor(ch, channelColors[ch]);
  }
}

void
TracerWidget::initToolBox()
{
  //默认标题设置
  ui->OscTitleLineEdit->setText(defautGraphTitle);
  //默认x轴标题设置
  ui->xTitleLineEdit->setText(defaultXTitle);
  //默认y轴标题设置
  ui->yTitleLineEdit->setText(defaultYTitle);
}

void
TracerWidget::initConnections()
{
  using tw = TracerWidget;
  using QCP = QCustomPlot; //省略一下
  connect(m_xcomm, &XComm::tracerCmd, this, &tw::slotProccessCmd);
  //关联选中图例与线条
  connect(ui->customPlot,
          &QCP::selectionChangedByUser,
          this,
          &tw::slotSelectionChanged);
  //存在多个重载，使用指针函数明确具体函数
  // x轴范围变化
  void (QCPAxis::*xRangeChangedFunc)(const QCPRange&) = &QCPAxis::rangeChanged;
  connect(
    ui->customPlot->xAxis, xRangeChangedFunc, this, &tw::slotxAxisChanged);
  // y轴范围变化
  void (QCPAxis::*yRangeChangedFunc)(const QCPRange&) = &QCPAxis::rangeChanged;
  connect(
    ui->customPlot->yAxis, yRangeChangedFunc, this, &tw::slotyAxisChanged);
  //双击legend
  void (QCP::*legendDoubleClickFunc)(
    QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*) = &QCP::legendDoubleClick;
  connect(
    ui->customPlot, legendDoubleClickFunc, this, &tw::slotLegendDoubleClick);
  // 双击坐标轴标题
  void (QCP::*axisDoubleClickFunc)(
    QCPAxis*, QCPAxis::SelectablePart, QMouseEvent*) = &QCP::axisDoubleClick;
  connect(
    ui->customPlot, axisDoubleClickFunc, this, &tw::slotAxisLabelDoubleClick);
  // 双击图标标题
  connect(
    m_title, &QCPTextElement::doubleClicked, this, &tw::slotTitleDoubleClick);
  //鼠标点击某个轴
  //可以在该轴的方向上缩放
  //鼠标点击
  connect(ui->customPlot, &QCP::mousePress, this, &tw::slotMousePress);
  //滚轮移动
  connect(ui->customPlot, &QCP::mouseWheel, this, &tw::slotMouseWheel);
  //更新统计信息表
  connect(m_updateStatsTimer, &QTimer::timeout, this, &tw::slotUpdateStats);
}

void
TracerWidget::storeTransformedData(Channels waitForUdpdateCh,
                                   const QByteArray& data,
                                   DataTransformType type)
{
  using namespace DriverDataType;
  // clear data storage before append
  m_points[waitForUdpdateCh].clear();
  // read config from run config
  RunConfigType runConfig = m_runConfiger->getRunConfig();
  // sample ponits
  int samplePoints = runConfig.data().m_samplePoints;
  // reserve for avoiding frequent memory allocate
  m_points[waitForUdpdateCh].reserve(samplePoints);
  // get setting sample cycles according to frequency
  double sampleCycles = 1.0 / runConfig.data().m_sampleFreq;
  // total length, 数据总字节长度
  size_t totalByteLen = 0;
  // cycle step，循环步长，数据类型有关
  size_t step = 0;
  // transFormFactor，变换系数
  double transFormFactor = 0;
  //根据数据类型计算变换系数
  switch (type) {
    case TracerWidget::CURRENT:
      totalByteLen = samplePoints * sizeof(currentDataType);
      step = sizeof(currentDataType);
      transFormFactor = currentTransformFactor / scaleFactor;
      break;
    case TracerWidget::VELOCITY:
      totalByteLen = samplePoints * sizeof(velocityDataType);
      step = sizeof(velocityDataType);
      transFormFactor = velocityTransformFactor / scaleFactor;
      break;
    case TracerWidget::POSITION:
      totalByteLen = samplePoints * sizeof(positionDataType);
      step = sizeof(positionDataType);
      transFormFactor = positionTransformFactor;
      break;
    case TracerWidget::ENCODER:
      totalByteLen = samplePoints * sizeof(encoderDataType);
      step = sizeof(encoderDataType);
      transFormFactor = encoderTransformFactor;
      break;
    default:
      break;
  }
  //检查数据长度
  Q_ASSERT_X((size_t)data.size() == totalByteLen,
             "storeTransformedData",
             "recv data length is wrong");
  //将QByteArray转成相应的数据储存到存储区
  //根据数据的字节长度，取一定的步长（字节数）
  //将数据从byteArray中取出并转成double
  for (size_t indexOfArray = 0, indexOfPoint = 0; indexOfArray < totalByteLen;
       indexOfArray += step, indexOfPoint++) {
    //将字节数据转成整型
    int y = BitConverter::ba2Int(data.mid(indexOfArray, step));
    double transformedY = y * transFormFactor; // y进行变换
    double x = indexOfPoint * sampleCycles; // x根据采样周期和采样点数变换到时间
    m_points[waitForUdpdateCh].append(QCPGraphData(x, transformedY));
  }
  // show on plot
  relaceChannelData(waitForUdpdateCh, m_points[waitForUdpdateCh]);
}

void
TracerWidget::slotProccessCmd(const quint16 cmd, const QByteArray& data)
{
  Channels waitForUdpdateCh;
  if (m_chReqOrderQueue.isEmpty()) {
    //如果出现queue为空但是接收到数，可能是用户由控制台发起指令
    qDebug(
      "[tracer]recv data count > req data count, send request from console?");
  } else { // queue is not empty
    //接收到数据时按照queue中储存的通道顺序依次放入使能的通道
    //其顺序和发送请求时使能的通道的顺序一致（ch1-8）
    waitForUdpdateCh = m_chReqOrderQueue.dequeue();
  }
  //根据不同的数据类型进行变换，然后储存到通道数据区里面
  switch (cmd) {
    case XComm::TRACER_REQ_VELOCITY_FDB:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::VELOCITY);
      break;
    case XComm::TRACER_REQ_VELOCITY_REF:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::VELOCITY);
      break;
    case XComm::TRACER_REQ_VELOCITY_OUT:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::VELOCITY);
      break;
    case XComm::TRACER_REQ_CURRENT_FDB:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::CURRENT);
      break;
    case XComm::TRACER_REQ_CURRENT_REF:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::CURRENT);
      break;
    case XComm::TRACER_REQ_CURRENT_OUT:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::CURRENT);
      break;
    case XComm::TRACER_REQ_POSITION_FDB:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::POSITION);
      break;
    case XComm::TRACER_REQ_POSITION_REF:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::POSITION);
      break;
    case XComm::TRACER_REQ_POSITION_OUT:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::POSITION);
      break;
    case XComm::TRACER_REQ_ENCODER_CLOCK:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::ENCODER);
      break;
    case XComm::TRACER_REQ_CURRENT_U:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::CURRENT);
      break;
    case XComm::TRACER_REQ_CURRENT_V:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::CURRENT);
      break;
    case XComm::TRACER_REQ_CURRENT_W:
      storeTransformedData(waitForUdpdateCh, data, TracerWidget::CURRENT);
      break;
    default:
      break;
  }
}

void
TracerWidget::openChannel(Channels channel)
{
  // 显示
  ui->customPlot->graph(channel)->setVisible(true);
  ui->customPlot->replot();
}

void
TracerWidget::closeChannel(Channels channel)
{
  //隐藏
  ui->customPlot->graph(channel)->setVisible(false);
}

void
TracerWidget::addDataToCh(Channels channel, const qreal x, const qreal y)
{
  //看不见 == 通道关闭
  if (!ui->customPlot->graph(channel)->visible()) {
    return;
  }
  //添加数据
  ui->customPlot->graph(channel)->addData(x, y);
  // 重画图像
  ui->customPlot->replot();
}

void
TracerWidget::relaceChannelData(Channels channel,
                                const QVector<QCPGraphData>& data)
{
  //看不见 == 通道关闭
  if (!ui->customPlot->graph(channel)->visible()) {
    return;
  }
  //替换数据
  // sortkey设置为true保证key是升序的，可以优化速度
  ui->customPlot->graph(channel)->data()->set(data, true);
  // 重画图像
  ui->customPlot->replot();
}

auto
TracerWidget::selectChannel() -> Channels
{
  //选择要的通道
  QStringList items; // ComboBox 列表的内容
  for (int ch = 0; ch < ui->customPlot->graphCount(); ch++) {
    if (ui->customPlot->graph(ch)->visible()) {
      items << ui->customPlot->graph(ch)->name();
    }
  }
  if (items.isEmpty()) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("无可用通道"));
    return ChUNK;
  }
  int curIndex = 0;     //初始选择项
  bool editable = true; // ComboBox是否可编辑
  bool ok = false;
  QString text = QInputDialog::getItem(this,
                                       QStringLiteral("需要的通道数据"),
                                       QStringLiteral("请选择通道"),
                                       items,
                                       curIndex,
                                       editable,
                                       &ok,
                                       Qt::MSWindowsFixedSizeDialogHint);
  if (!ok || text.isEmpty()) { // 错误
    return ChUNK;
  }
  //返回选择的通道
  // graph number is channel number - 1
  int ch = text.midRef(2, 1).toInt() - 1;
  return static_cast<Channels>(ch);
}

void
TracerWidget::on_startButton_clicked()
{
  if (m_xcomm->connectStatus() == XComm::COMM_IDLE) {
    // 未连接
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("未连接驱动器"));
    return;
  }
  if (m_xcomm->MotorStatus() == XComm::MOTOR_RUN) {
    // if motor is running , no action
    QMessageBox::warning(this,
                         QStringLiteral("警告"),
                         QStringLiteral("电机已在运行，不要重复发送指令！"));
    return;
  }
  //运行参数配置对话框，提示当前运行模式
  m_runConfiger->setRunModeInfo(
    m_controllerConfiger->m_ctrllerConfigWidget->currentDSPRunMode());
  if (m_runConfiger->exec() == QDialog::Rejected) {
    return;
  }
  //添加运行参数
  DriverDataType::RunConfigType runConfig = m_runConfiger->getRunConfig();
  // if motor is stop，start it
  m_xcomm->startMotor(runConfig);
}

void
TracerWidget::on_stopButton_clicked()
{
  if (m_xcomm->connectStatus() == XComm::COMM_IDLE) {
    // 未连接
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("未连接驱动器"));
    return;
  }
  if (m_xcomm->MotorStatus() == XComm::MOTOR_RUN) {
    // if motor is running，stop it
    m_xcomm->stopMotor();
  }
}

int
TracerWidget::on_configButton_clicked()
{
  //控制器参数配置对话框
  int ret = m_controllerConfiger->exec();
  return ret; //返回值是enum QDialog::DialogCode
}

void
TracerWidget::on_readButton_clicked()
{
  if (m_xcomm->connectStatus() == XComm::COMM_IDLE) {
    // 未连接
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("未连接驱动器"));
    return;
  }
  //检查是否有可用通道
  int visibleCount = 0;
  for (int index = 0; index < ui->customPlot->graphCount(); index++) {
    if (ui->customPlot->graph(index)->visible()) {
      visibleCount++;
    }
  }
  if (visibleCount == 0) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("无可用通道"));
    return;
  }
  //确认对话框
  QMessageBox msgBox(this);
  msgBox.setFixedSize(200, 200);
  msgBox.setWindowTitle("提示");
  msgBox.setText(QStringLiteral("将从驱动器读取通道数据?"));
  msgBox.setInformativeText(QStringLiteral("大约需要1~2s"));
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Cancel) {
    return;
  }
  //防止用户多次操作
  ui->readButton->setEnabled(false);
  //读取一下通道设定内容
  readChBoxContent();
  //检查接收数据数量是否与请求的数量一致
  //如果请求的数据与接收的数据在数量上一致
  // m_chOrderQueue应该是空的
  //如果m_chOrderQueue应该是空的不为空
  //很可能没收到数据或者部分数据未收到
  QTimer::singleShot(defaultCheckRecvInterval, this, [=]() {
    if (m_chReqOrderQueue.isEmpty()) {
      QMessageBox::information(
        this, QStringLiteral("提示"), QStringLiteral("读取数据成功"));
    } else {
      QMessageBox::warning(
        this, QStringLiteral("错误"), QStringLiteral("读取数据失败"));
      //错误数据，全部清理掉
      on_clearDataButton_clicked();
    }
    ui->readButton->setEnabled(true);
  });
}

auto
TracerWidget::chComboIndexMapToCmd(int index) -> XComm::CmdType
{
  //通道设定下拉框映射到命令
  XComm::CmdType cmd;
  switch (index) {
    case 0:
      cmd = XComm::TRACER_REQ_VELOCITY_REF;
      break;
    case 1:
      cmd = XComm::TRACER_REQ_VELOCITY_FDB;
      break;
    case 2:
      cmd = XComm::TRACER_REQ_VELOCITY_OUT;
      break;
    case 3:
      cmd = XComm::TRACER_REQ_POSITION_REF;
      break;
    case 4:
      cmd = XComm::TRACER_REQ_POSITION_FDB;
      break;
    case 5:
      cmd = XComm::TRACER_REQ_POSITION_OUT;
      break;
    case 6:
      cmd = XComm::TRACER_REQ_CURRENT_REF;
      break;
    case 7:
      cmd = XComm::TRACER_REQ_CURRENT_FDB;
      break;
    case 8:
      cmd = XComm::TRACER_REQ_CURRENT_OUT;
      break;
    case 9:
      cmd = XComm::TRACER_REQ_ENCODER_CLOCK;
      break;
    case 10:
      cmd = XComm::TRACER_REQ_CURRENT_U;
      break;
    case 11:
      cmd = XComm::TRACER_REQ_CURRENT_V;
      break;
    case 12:
      cmd = XComm::TRACER_REQ_CURRENT_W;
      break;
    default:
      cmd = XComm::CMD_UNKNOWN;
      qFatal("unknown index map to command");
      break;
  }
  return cmd;
}

void
TracerWidget::readChBoxContent()
{
  //先将队列清空
  //如果请求未得到应答
  //请求的通道的数据将储存在queue中
  //所以，在下一次请求前清除之前未得到应答的数据的顺序
  m_chReqOrderQueue.clear();
  //依次检查8个下拉框，依序发送请求命令
  //将发送命令的按顺序存入queue
  //接收到数据时按照queue中储存的顺序依次放入通道
  if (ui->ch1ComboBox->isEnabled()) {
    m_chReqOrderQueue.enqueue(Ch1);
    m_xcomm->command(chComboIndexMapToCmd(ui->ch1ComboBox->currentIndex()),
                     QByteArray());
  }
  if (ui->ch2ComboBox->isEnabled()) {
    m_chReqOrderQueue.enqueue(Ch2);
    m_xcomm->command(chComboIndexMapToCmd(ui->ch2ComboBox->currentIndex()),
                     QByteArray());
  }
  if (ui->ch3ComboBox->isEnabled()) {
    m_chReqOrderQueue.enqueue(Ch3);
    m_xcomm->command(chComboIndexMapToCmd(ui->ch3ComboBox->currentIndex()),
                     QByteArray());
  }
  if (ui->ch4ComboBox->isEnabled()) {
    m_chReqOrderQueue.enqueue(Ch4);
    m_xcomm->command(chComboIndexMapToCmd(ui->ch4ComboBox->currentIndex()),
                     QByteArray());
  }
  if (ui->ch5ComboBox->isEnabled()) {
    m_chReqOrderQueue.enqueue(Ch5);
    m_xcomm->command(chComboIndexMapToCmd(ui->ch5ComboBox->currentIndex()),
                     QByteArray());
  }
  if (ui->ch6ComboBox->isEnabled()) {
    m_chReqOrderQueue.enqueue(Ch6);
    m_xcomm->command(chComboIndexMapToCmd(ui->ch6ComboBox->currentIndex()),
                     QByteArray());
  }
  if (ui->ch7ComboBox->isEnabled()) {
    m_chReqOrderQueue.enqueue(Ch7);
    m_xcomm->command(chComboIndexMapToCmd(ui->ch7ComboBox->currentIndex()),
                     QByteArray());
  }
  if (ui->ch8ComboBox->isEnabled()) {
    m_chReqOrderQueue.enqueue(Ch8);
    m_xcomm->command(chComboIndexMapToCmd(ui->ch8ComboBox->currentIndex()),
                     QByteArray());
  }
}

void
TracerWidget::on_autoSetButton_clicked()
{
  //调整XY轴的范围，使之能显示出可见曲线
  ui->customPlot->rescaleAxes(true);
  ui->customPlot->replot();
}

void
TracerWidget::on_closeAllchannelButton_clicked()
{ //通道全关了

  ui->ch1Box->setChecked(false);
  ui->ch2Box->setChecked(false);
  ui->ch3Box->setChecked(false);
  ui->ch4Box->setChecked(false);
  ui->ch5Box->setChecked(false);
  ui->ch6Box->setChecked(false);
  ui->ch7Box->setChecked(false);
  ui->ch8Box->setChecked(false);
}

void
TracerWidget::on_showAllchannelButton_clicked()
{ //通道全开了
  ui->ch1Box->setChecked(true);
  ui->ch2Box->setChecked(true);
  ui->ch3Box->setChecked(true);
  ui->ch4Box->setChecked(true);
  ui->ch5Box->setChecked(true);
  ui->ch6Box->setChecked(true);
  ui->ch7Box->setChecked(true);
  ui->ch8Box->setChecked(true);
}

void
TracerWidget::on_xMaxDoubleSpinBox_valueChanged(double xMax)
{
  // if user is dragging plot, we don't want to replot twice
  if (qAbs(qAbs(ui->customPlot->xAxis->range().upper) - qAbs(xMax)) > 0.01) {
    ui->customPlot->xAxis->setRangeUpper(xMax);
    ui->customPlot->replot();
  }
}

void
TracerWidget::on_xMinDoubleSpinBox_valueChanged(double xMin)
{
  // if user is dragging plot, we don't want to replot twice
  if (qAbs(qAbs(ui->customPlot->xAxis->range().lower) - qAbs(xMin)) > 0.01) {
    ui->customPlot->xAxis->setRangeLower(xMin);
    ui->customPlot->replot();
  }
}

void
TracerWidget::on_yMaxDoubleSpinBox_valueChanged(double yMax)
{
  // if user is dragging plot, we don't want to replot twice
  if (qAbs(qAbs(ui->customPlot->yAxis->range().upper) - qAbs(yMax)) > 0.01) {
    ui->customPlot->yAxis->setRangeUpper(yMax);
    ui->customPlot->replot();
  }
}

void
TracerWidget::on_yMinDoubleSpinBox_valueChanged(double yMin)
{
  // if user is dragging plot, we don't want to replot twice
  if (qAbs(qAbs(ui->customPlot->yAxis->range().lower) - qAbs(yMin)) > 0.01) {
    ui->customPlot->yAxis->setRangeLower(yMin);
    ui->customPlot->replot();
  }
}

void
TracerWidget::on_xTickSpinBox_valueChanged(int xTick)
{
  ui->customPlot->xAxis->ticker()->setTickCount(xTick);
  ui->customPlot->replot();
}

void
TracerWidget::on_yTickSpinBox_valueChanged(int yTick)
{
  ui->customPlot->yAxis->ticker()->setTickCount(yTick);
  ui->customPlot->replot();
}

void
TracerWidget::on_clearDataButton_clicked()
{
  for (int ch = 0; ch < ui->customPlot->graphCount(); ch++) {
    //只清除数据，不删除graph
    ui->customPlot->graph(ch)->data().data()->clear();
    m_points[ch].clear();
  }
  ui->customPlot->replot();
}

void
TracerWidget::on_defaultButton_clicked()
{
  //重设SpinBox座标轴范围
  ui->xMaxDoubleSpinBox->setValue(defalutMaxX);
  ui->xMinDoubleSpinBox->setValue(defalutMinX);
  ui->yMaxDoubleSpinBox->setValue(defalutMaxY);
  ui->yMinDoubleSpinBox->setValue(defalutMinY);
}

void
TracerWidget::on_exportDataButton_clicked()
{
  //选择需要保存的通道对话框
  Channels selectedCh = selectChannel();
  if (selectedCh == ChUNK) {
    // no select ch
    return;
  }
  // have data in ch ?
  if (ui->customPlot->graph(selectedCh)->dataCount() == 0) {
    // no data in channel
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("通道无数据"));
    return;
  }
  //保存文件对话框
  QDateTime dateTime(QDateTime::currentDateTime());
  QString format = QStringLiteral("yyyyMMdd-hhmmss");
  QString timeStr = dateTime.toString(format);
  QString fileName = QFileDialog::getSaveFileName(
    this,
    QStringLiteral("保存数据"),
    qApp->applicationDirPath() % ui->customPlot->graph(selectedCh)->name() %
      timeStr % QLatin1String(".csv"),
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
  Q_FOREACH (const auto& pt, m_points[selectedCh]) {
    QTextStream out(&file);
    out << pt.key << ',' << pt.value << "\n";
  }
  file.close();
  QMessageBox::information(
    this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
}

void
TracerWidget::on_exportImgButton_clicked()
{
  //保存文件对话框
  QDateTime dateTime(QDateTime::currentDateTime());
  QString format = QStringLiteral("yyyyMMdd-hhmmss");
  QString timeStr = dateTime.toString(format);
  QString fileName = QFileDialog::getSaveFileName(
    this,
    QStringLiteral("保存数据图像"),
    qApp->applicationDirPath() % timeStr % QLatin1String(".png"),
    QStringLiteral("Images (*.png)"));
  if (fileName.isEmpty()) {
    return;
  }
  //保存文件
  if (ui->customPlot->savePng(fileName)) { // 打开/创建图像成功
    QMessageBox::information(
      this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
  } else { // 打开/创建图像失败
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("创建图像失败！"));
  }
}

void
TracerWidget::on_importDataButton_clicked()
{
  //选择需要打开的通道对话框
  Channels selectedCh = selectChannel();
  if (selectedCh == ChUNK) {
    // no select ch
    return;
  }
  //打开文件对话框
  QString fileName =
    QFileDialog::getOpenFileName(this,
                                 QStringLiteral("载入数据"),
                                 qApp->applicationDirPath(),
                                 QStringLiteral("Files (*.csv *.txt)"));
  if (fileName.isEmpty()) {
    return;
  }
  // open file
  QFile file(fileName);
  //以只读方式打开
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("打开文件失败！"));
    return;
  }
  // is empty ?
  if (file.size() == 0) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("文件为空！"));
    file.close();
    return;
  }
  // clear storage before append data
  m_points[selectedCh].clear();
  // reserve for avoiding frequent memory allocate
  m_points[selectedCh].reserve(8192);
  // read data & write in osc
  QTextStream in(&file);
  while (!in.atEnd()) {
    // check data is correct?
    bool isOk_x = false;
    bool isOk_y = false;
    QStringList xy = in.readLine().split(',');
    if (xy.length() != 2) {
      //文件格式错误，每行数据 != 2
      file.close();
      m_points[selectedCh].clear();
      QMessageBox::warning(this,
                           QStringLiteral("错误"),
                           QStringLiteral("文件格式错误，每行数据 != 2 ！"));
      return;
    }
    qreal x = xy[0].toDouble(&isOk_x); // convert to qreal
    qreal y = xy[1].toDouble(&isOk_y); // convert to qreal
    if (!isOk_x || !isOk_y) {
      //文件数据错误，无法转换成double
      file.close();
      m_points[selectedCh].clear();
      QMessageBox::warning(this,
                           QStringLiteral("错误"),
                           QStringLiteral("文件数据错误，无法转换成double！"));
      return;
    }
    // data isOk
    m_points[selectedCh].append(QCPGraphData(x, y));
  }
  file.close();
  // display on OSC
  relaceChannelData(selectedCh, m_points[selectedCh]);
  QMessageBox::information(
    this, QStringLiteral("提示"), QStringLiteral("读取数据成功"));
  return;
}

void
TracerWidget::setChannelDispay(Channels ch, int state)
{
  bool enable = false;
  switch (state) {
    case Qt::Unchecked:
      closeChannel(ch);
      enable = false;
      break;
    case Qt::PartiallyChecked:
      break;
    case Qt::Checked:
      openChannel(ch);
      enable = true;
      break;
    default:
      break;
  }
  //曲线对应图例是否显示
  QCPGraph* graph = ui->customPlot->graph(ch);
  auto* item = ui->customPlot->legend->itemWithPlottable(graph);
  item->setVisible(enable);
  ui->customPlot->replot();
}

void
TracerWidget::on_ch1Box_stateChanged(int state)
{
  setChannelDispay(Ch1, state);
  //通道下拉框是否可用
  ui->ch1ComboBox->setEnabled(state == 2 ? true : false);
}

void
TracerWidget::on_ch2Box_stateChanged(int state)
{
  setChannelDispay(Ch2, state);
  //通道下拉框是否可用
  ui->ch2ComboBox->setEnabled(state == 2 ? true : false);
}

void
TracerWidget::on_ch3Box_stateChanged(int state)
{
  setChannelDispay(Ch3, state);
  //通道下拉框是否可用
  ui->ch3ComboBox->setEnabled(state == 2 ? true : false);
}

void
TracerWidget::on_ch4Box_stateChanged(int state)
{
  setChannelDispay(Ch4, state);
  //通道下拉框是否可用
  ui->ch4ComboBox->setEnabled(state == 2 ? true : false);
}

void
TracerWidget::on_ch5Box_stateChanged(int state)
{
  setChannelDispay(Ch5, state);
  //通道下拉框是否可用
  ui->ch5ComboBox->setEnabled(state == 2 ? true : false);
}

void
TracerWidget::on_ch6Box_stateChanged(int state)
{
  setChannelDispay(Ch6, state);
  //通道下拉框是否可用
  ui->ch6ComboBox->setEnabled(state == 2 ? true : false);
}

void
TracerWidget::on_ch7Box_stateChanged(int state)
{
  setChannelDispay(Ch7, state);
  //通道下拉框是否可用
  ui->ch7ComboBox->setEnabled(state == 2 ? true : false);
}

void
TracerWidget::on_ch8Box_stateChanged(int state)
{
  setChannelDispay(Ch8, state);
  //通道下拉框是否可用
  ui->ch8ComboBox->setEnabled(state == 2 ? true : false);
}

void
TracerWidget::on_legendBox_stateChanged(int state)
{
  switch (state) {
    case Qt::Unchecked:
      ui->customPlot->legend->setVisible(false);
      break;
    case Qt::PartiallyChecked:
      break;
    case Qt::Checked:
      ui->customPlot->legend->setVisible(true);
      break;
    default:
      break;
  }
  ui->customPlot->replot();
}

void
TracerWidget::on_legendPosComboBox_currentIndexChanged(int index)
{
  switch (index) {
    case BottomRight: //右下
      ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignBottom | Qt::AlignRight);
      break;
    case TopRight: //右上
      ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignTop | Qt::AlignRight);
      break;
    case BottomLeft: //左下
      ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignBottom | Qt::AlignLeft);
      break;
    case TopLeft: //左上
      ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignTop | Qt::AlignLeft);
      break;
    default:
      break;
  }
  ui->customPlot->replot();
}

void
TracerWidget::on_openGLBox_stateChanged(int state)
{
  switch (state) {
    case Qt::Unchecked:
      ui->customPlot->setOpenGl(false);
      break;
    case Qt::PartiallyChecked:
      break;
    case Qt::Checked:
      ui->customPlot->setOpenGl(true);
      break;
    default:
      break;
  }
  ui->customPlot->replot();
}

void
TracerWidget::setDarkTheme()
{
  // set some pens, brushes and backgrounds:
  // base
  ui->customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
  ui->customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
  // tick
  ui->customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
  ui->customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
  // sub tick
  ui->customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
  ui->customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
  // tick label
  ui->customPlot->xAxis->setTickLabelColor(Qt::white);
  ui->customPlot->yAxis->setTickLabelColor(Qt::white);
  // grid
  ui->customPlot->xAxis->grid()->setPen(
    QPen(QColor(140, 140, 140), 1, Qt::DotLine));
  ui->customPlot->yAxis->grid()->setPen(
    QPen(QColor(140, 140, 140), 1, Qt::DotLine));
  // subgrid
  ui->customPlot->xAxis->grid()->setSubGridPen(
    QPen(QColor(80, 80, 80), 1, Qt::DotLine));
  ui->customPlot->yAxis->grid()->setSubGridPen(
    QPen(QColor(80, 80, 80), 1, Qt::DotLine));
  // zero line
  ui->customPlot->xAxis->grid()->setZeroLinePen(QPen(Qt::gray, 1));
  ui->customPlot->yAxis->grid()->setZeroLinePen(QPen(Qt::gray, 1));
  // axis ending
  ui->customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  ui->customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  // back ground
  QLinearGradient plotGradient;
  plotGradient.setStart(0, 0);
  plotGradient.setFinalStop(0, 250);
  plotGradient.setColorAt(0, QColor(100, 100, 100));
  plotGradient.setColorAt(1, QColor(60, 60, 60));
  ui->customPlot->setBackground(plotGradient);
  // axis rect
  QLinearGradient axisRectGradient;
  axisRectGradient.setStart(0, 0);
  axisRectGradient.setFinalStop(0, 300);
  axisRectGradient.setColorAt(0, QColor(90, 90, 90));
  axisRectGradient.setColorAt(1, QColor(55, 55, 55));
  ui->customPlot->axisRect()->setBackground(axisRectGradient);
  // title
  m_title->setTextColor(Qt::white);
  // axis label
  ui->customPlot->xAxis->setLabelColor(Qt::white);
  ui->customPlot->yAxis->setLabelColor(Qt::white);
  ui->customPlot->replot();
}

void
TracerWidget::setLightTheme()
{
  // set some pens, brushes and backgrounds:
  // base
  ui->customPlot->xAxis->setBasePen(QPen(Qt::black, 1));
  ui->customPlot->yAxis->setBasePen(QPen(Qt::black, 1));
  // tick
  ui->customPlot->xAxis->setTickPen(QPen(Qt::black, 1));
  ui->customPlot->yAxis->setTickPen(QPen(Qt::black, 1));
  // sub tick
  ui->customPlot->xAxis->setSubTickPen(QPen(Qt::black, 1));
  ui->customPlot->yAxis->setSubTickPen(QPen(Qt::black, 1));
  // tick label
  ui->customPlot->xAxis->setTickLabelColor(Qt::black);
  ui->customPlot->yAxis->setTickLabelColor(Qt::black);
  // grid
  ui->customPlot->xAxis->grid()->setPen(
    QPen(QColor(40, 40, 40), 1, Qt::DotLine));
  ui->customPlot->yAxis->grid()->setPen(
    QPen(QColor(40, 40, 40), 1, Qt::DotLine));
  // subgrid
  ui->customPlot->xAxis->grid()->setSubGridPen(
    QPen(QColor(120, 120, 120), 1, Qt::DotLine));
  ui->customPlot->yAxis->grid()->setSubGridPen(
    QPen(QColor(120, 120, 120), 1, Qt::DotLine));
  // zero line
  ui->customPlot->xAxis->grid()->setZeroLinePen(QPen(Qt::gray, 1));
  ui->customPlot->yAxis->grid()->setZeroLinePen(QPen(Qt::gray, 1));
  // axis ending
  ui->customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  ui->customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  // back ground
  ui->customPlot->setBackground(QBrush(Qt::white));
  // axis rect
  ui->customPlot->axisRect()->setBackground(QBrush(Qt::white));
  // title
  m_title->setTextColor(Qt::black);
  // axis label
  ui->customPlot->xAxis->setLabelColor(Qt::black);
  ui->customPlot->yAxis->setLabelColor(Qt::black);
  ui->customPlot->replot();
}

void
TracerWidget::on_themeBox_currentIndexChanged(int index)
{
  switch (index) {
    case LightTheme:
      setLightTheme();
      break;
    case DarkTheme:
      setDarkTheme();
      break;
    default:
      break;
  }
}

void
TracerWidget::on_tracerCheckBox_stateChanged(int state)
{
  switch (state) {
    case Qt::Unchecked:
      ui->customPlot->setTracerVisible(false);
      break;
    case Qt::PartiallyChecked:
      break;
    case Qt::Checked:
      ui->customPlot->setTracerVisible(true);
      break;
    default:
      break;
  }
}

void
TracerWidget::on_FFTButton_clicked()
{
  // 选择需要FFT的通道对话框
  Channels selectedCh = selectChannel();
  if (selectedCh == ChUNK) {
    // no select ch
    return;
  }
  // check whether have data in channel
  if (ui->customPlot->graph(selectedCh)->dataCount() == 0) {
    // no data in channel
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("通道无数据"));
    return;
  }
  // TBC
  int ret = m_fftWindow->doFFT(m_points[selectedCh]);
  if (!ret) { // return 0 no error, show result
    m_fftWindow->show();
  }
}

void
TracerWidget::on_statsButton_clicked()
{
  if (m_statisticsWindow->isVisible()) {
    m_statisticsWindow->close();
    //停止刷新统计信息
    m_updateStatsTimer->stop();
  } else {
    m_statisticsWindow->updateStatistics(m_points);
    m_statisticsWindow->show();
    //刷新统计信息
    m_updateStatsTimer->start(defaultUpdateStatusInterval);
  }
}

void
TracerWidget::slotUpdateStats()
{
  if (!m_statisticsWindow->isVisible()) {
    //看不见停止刷新统计信息
    m_updateStatsTimer->stop();
  }
  m_statisticsWindow->updateStatistics(m_points);
}

void
TracerWidget::on_OscTitleLineEdit_editingFinished()
{
  m_title->setText(ui->OscTitleLineEdit->text());
  ui->customPlot->replot();
}

void
TracerWidget::on_xTitleLineEdit_editingFinished()
{
  ui->customPlot->xAxis->setLabel(ui->xTitleLineEdit->text());
  ui->customPlot->replot();
}

void
TracerWidget::on_yTitleLineEdit_editingFinished()
{
  ui->customPlot->yAxis->setLabel(ui->yTitleLineEdit->text());
  ui->customPlot->replot();
}

void
TracerWidget::slotSelectionChanged()
{
  for (int i = 0; i < ui->customPlot->graphCount(); ++i) {
    QCPGraph* graph = ui->customPlot->graph(i);
    auto* item = ui->customPlot->legend->itemWithPlottable(graph);
    //如果图像或者其对应的图例被选中，则标记他们
    if (item->selected() || graph->selected()) {
      item->setSelected(true);
      QPen pen;
      pen.setWidth(3);
      pen.setColor(Qt::blue);
      graph->selectionDecorator()->setPen(pen);
      //标记曲线
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void
TracerWidget::setSubGrid(bool enable)
{
  ui->customPlot->xAxis->grid()->setSubGridVisible(enable);
  ui->customPlot->yAxis->grid()->setSubGridVisible(enable);
  ui->customPlot->replot();
}

void
TracerWidget::on_subAxisCheckBox_stateChanged(int state)
{
  switch (state) {
    case Qt::Unchecked:
      setSubGrid(false);
      break;
    case Qt::PartiallyChecked:
      break;
    case Qt::Checked:
      setSubGrid(true);
      break;
    default:
      break;
  }
}

void
TracerWidget::slotxAxisChanged(const QCPRange& range)
{
  // 重设SpinBox
  ui->xMaxDoubleSpinBox->setValue(range.upper);
  ui->xMinDoubleSpinBox->setValue(range.lower);
}

void
TracerWidget::slotyAxisChanged(const QCPRange& range)
{
  ui->yMaxDoubleSpinBox->setValue(range.upper);
  ui->yMinDoubleSpinBox->setValue(range.lower);
}

void
TracerWidget::slotLegendDoubleClick(QCPLegend* legend,
                                    QCPAbstractLegendItem* item)
{
  // only react if item was clicked (user could have clicked on border
  // padding of legend where there is no item, then item is 0)
  if (!item) {
    return;
  }
  for (int i = 0; i < ui->customPlot->graphCount(); ++i) {
    QCPGraph* graph = ui->customPlot->graph(i);
    //找出legend对应的graph
    auto* item = legend->itemWithPlottable(graph);
    if (!item->selected()) {
      // legend没被选中
      continue;
    }
    //读取并显示一下当前曲线的style
    auto lastLineStyle = graph->lineStyle();
    auto lastScatterStyle = graph->scatterStyle();
    SelectLineStyleDialog dialog;
    dialog.restoreStyle(lastLineStyle, lastScatterStyle);
    int ret = dialog.exec();
    if (ret == QDialog::Rejected) {
      return;
    }
    QPair<int, QCPScatterStyle> style = dialog.getStyle();
    // 1st is line style, 2nd is scatter style
    graph->setLineStyle(static_cast<QCPGraph::LineStyle>(style.first));
    graph->setScatterStyle(style.second);
    ui->customPlot->replot();
  }
}

void
TracerWidget::slotTitleDoubleClick(QMouseEvent* event)
{
  Q_UNUSED(event)
  auto* title = qobject_cast<QCPTextElement*>(sender());
  if (!title) {
    return;
  }
  // Set the plot title by double clicking on it
  bool ok;
  QString newTitle = QInputDialog::getText(this,
                                           QStringLiteral("设置图标标题"),
                                           QStringLiteral("新标题："),
                                           QLineEdit::Normal,
                                           title->text(),
                                           &ok,
                                           Qt::MSWindowsFixedSizeDialogHint);
  if (ok) {
    title->setText(newTitle);
    ui->OscTitleLineEdit->setText(newTitle);
    ui->customPlot->replot();
  }
}

void
TracerWidget::slotAxisLabelDoubleClick(QCPAxis* axis,
                                       QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part != QCPAxis::spAxisLabel) {
    return;
  }
  // only react when the actual axis label is
  // clicked, not tick label or axis backbone
  bool ok;
  QString newLabel = QInputDialog::getText(this,
                                           QStringLiteral("设置坐标轴标题"),
                                           QStringLiteral("新标题："),
                                           QLineEdit::Normal,
                                           axis->label(),
                                           &ok,
                                           Qt::MSWindowsFixedSizeDialogHint);
  if (ok) {
    axis->setLabel(newLabel);
    if (axis->axisType() == QCPAxis::atBottom) {
      // X axis
      ui->xTitleLineEdit->setText(newLabel);
    } else if (axis->axisType() == QCPAxis::atLeft) {
      // Y axis
      ui->yTitleLineEdit->setText(newLabel);
    } else {
      qDebug() << "select unkown axis title";
    }
    ui->customPlot->replot();
  }
}

void
TracerWidget::slotMousePress()
{
  // if an axis is selected, only allow the direction of that axis to be
  // dragged if no axis is selected, both directions may be dragged
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
    ui->customPlot->axisRect()->setRangeDrag(
      ui->customPlot->xAxis->orientation());
  } else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
    ui->customPlot->axisRect()->setRangeDrag(
      ui->customPlot->yAxis->orientation());
  } else {
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
  }
}

void
TracerWidget::slotMouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be
  // zoomed if no axis is selected, both directions may be zoomed
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
    ui->customPlot->axisRect()->setRangeZoom(
      ui->customPlot->xAxis->orientation());
  } else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
    ui->customPlot->axisRect()->setRangeZoom(
      ui->customPlot->yAxis->orientation());
  } else {
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
  }
}

void
TracerWidget::on_ch1ComboBox_currentIndexChanged(int index)
{
  const static int CH = 1;
  QString newName = tr("CH%1-%2").arg(CH).arg(channelList[index]);
  ui->customPlot->graph(CH - 1)->setName(newName);
  m_statisticsWindow->setRowHeaderText(CH - 1, newName);
  ui->customPlot->replot();
}

void
TracerWidget::on_ch2ComboBox_currentIndexChanged(int index)
{
  const static int CH = 2;
  QString newName = tr("CH%1-%2").arg(CH).arg(channelList[index]);
  ui->customPlot->graph(CH - 1)->setName(newName);
  m_statisticsWindow->setRowHeaderText(CH - 1, newName);
  ui->customPlot->replot();
}

void
TracerWidget::on_ch3ComboBox_currentIndexChanged(int index)
{
  const static int CH = 3;
  QString newName = tr("CH%1-%2").arg(CH).arg(channelList[index]);
  ui->customPlot->graph(CH - 1)->setName(newName);
  m_statisticsWindow->setRowHeaderText(CH - 1, newName);
  ui->customPlot->replot();
}

void
TracerWidget::on_ch4ComboBox_currentIndexChanged(int index)
{
  const static int CH = 4;
  QString newName = tr("CH%1-%2").arg(CH).arg(channelList[index]);
  ui->customPlot->graph(CH - 1)->setName(newName);
  m_statisticsWindow->setRowHeaderText(CH - 1, newName);
  ui->customPlot->replot();
}

void
TracerWidget::on_ch5ComboBox_currentIndexChanged(int index)
{
  const static int CH = 5;
  QString newName = tr("CH%1-%2").arg(CH).arg(channelList[index]);
  ui->customPlot->graph(CH - 1)->setName(newName);
  m_statisticsWindow->setRowHeaderText(CH - 1, newName);
  ui->customPlot->replot();
}

void
TracerWidget::on_ch6ComboBox_currentIndexChanged(int index)
{
  const static int CH = 6;
  QString newName = tr("CH%1-%2").arg(CH).arg(channelList[index]);
  ui->customPlot->graph(CH - 1)->setName(newName);
  m_statisticsWindow->setRowHeaderText(CH - 1, newName);
  ui->customPlot->replot();
}

void
TracerWidget::on_ch7ComboBox_currentIndexChanged(int index)
{
  const static int CH = 7;
  QString newName = tr("CH%1-%2").arg(CH).arg(channelList[index]);
  ui->customPlot->graph(CH - 1)->setName(newName);
  m_statisticsWindow->setRowHeaderText(CH - 1, newName);
  ui->customPlot->replot();
}

void
TracerWidget::on_ch8ComboBox_currentIndexChanged(int index)
{
  const static int CH = 8;
  QString newName = tr("CH%1-%2").arg(CH).arg(channelList[index]);
  ui->customPlot->graph(CH - 1)->setName(newName);
  m_statisticsWindow->setRowHeaderText(CH - 1, newName);
  ui->customPlot->replot();
}

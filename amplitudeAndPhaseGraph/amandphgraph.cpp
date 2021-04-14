#include "amandphgraph.h"
#include "ui_amandphgraph.h"

AmAndPhGraph::AmAndPhGraph(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::AmAndPhGraph)
{
  ui->setupUi(this);

  initAmplitudeGraph();
  initPhaseGraph();
  initConnections();
}

AmAndPhGraph::~AmAndPhGraph()
{
  delete ui;
}

void
AmAndPhGraph::initAmplitudeGraph()
{
  // chart setting
  ui->amWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  // more performance/responsiveness during dragging
  ui->amWidget->setNoAntialiasingOnDrag(false);
  // Defines plotting hints that control the quality and speed of plotting
  // speed of plotting axis labels will be cached as pixmaps
  ui->amWidget->setPlottingHint(QCP::phCacheLabels, true);
  // 创建 1 个通道数据
  ui->amWidget->addGraph(ui->amWidget->xAxis, ui->amWidget->yAxis);
  ui->amWidget->graph(0)->setName(QStringLiteral("Amplitude"));
  //设置x（频率）为对数坐标轴
  QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
  ui->amWidget->xAxis->setTicker(logTicker);
  // set axis labels,  no need x axis title
  ui->amWidget->yAxis->setLabel(QStringLiteral("Amplitude(dB)"));
  //坐标轴范围、刻度
  ui->amWidget->xAxis->setRange(1, 10000);
  ui->amWidget->yAxis->setRange(-100, 100);
  //设置x轴缩放为对数形式
  ui->amWidget->xAxis->setScaleType(QCPAxis::stLogarithmic);
  // 刻度线数量
  ui->amWidget->xAxis->ticker()->setTickCount(4);
  ui->amWidget->yAxis->ticker()->setTickCount(10);
  //坐标轴精度
  ui->amWidget->xAxis->setNumberPrecision(0);
  ui->amWidget->yAxis->setNumberPrecision(1);
  //坐标轴格式，科学计数法，使用乘号表示
  ui->amWidget->xAxis->setNumberFormat("ebc");
  //纯数字，固定格式
  ui->amWidget->yAxis->setNumberFormat("f");
  //初始化游标,在此之前先初始化图像
  ui->amWidget->initGraphTracer();
}

void
AmAndPhGraph::initPhaseGraph()
{
  // chart setting
  ui->phWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  // more performance/responsiveness during dragging
  ui->phWidget->setNoAntialiasingOnDrag(false);
  // Defines plotting hints that control the quality and speed of plotting
  // speed of plotting axis labels will be cached as pixmaps
  ui->phWidget->setPlottingHint(QCP::phCacheLabels, true);
  // 创建 1 个通道数据
  ui->phWidget->addGraph(ui->phWidget->xAxis, ui->phWidget->yAxis);
  ui->phWidget->graph(0)->setName(QStringLiteral("Phase"));
  //设置x（频率）为对数坐标轴
  QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
  ui->phWidget->xAxis->setTicker(logTicker);
  // set axis labels
  ui->phWidget->xAxis->setLabel(QStringLiteral("frequency(Hz)"));
  ui->phWidget->yAxis->setLabel(QStringLiteral("Phase(deg)"));
  //坐标轴范围、刻度
  ui->phWidget->xAxis->setRange(1, 10000);
  ui->phWidget->yAxis->setRange(-280, -80);
  //设置x轴缩放为对数形式
  ui->phWidget->xAxis->setScaleType(QCPAxis::stLogarithmic);
  // 刻度线数量
  ui->phWidget->xAxis->ticker()->setTickCount(4);
  ui->phWidget->yAxis->ticker()->setTickCount(10);
  //坐标轴精度
  ui->phWidget->xAxis->setNumberPrecision(0);
  ui->phWidget->yAxis->setNumberPrecision(1);
  //坐标轴格式，科学计数法，使用乘号表示
  ui->phWidget->xAxis->setNumberFormat("ebc");
  //纯数字，固定格式
  ui->phWidget->yAxis->setNumberFormat("f");
  //初始化游标,在此之前先初始化图像
  ui->phWidget->initGraphTracer();
}

void
AmAndPhGraph::initConnections()
{
  //幅频图和相拼图的tracer绑定，实现同步移动
  connect(
    ui->amWidget, &Graph::tracerMove, ui->phWidget, &Graph::slotSyncTracer);
  connect(
    ui->phWidget, &Graph::tracerMove, ui->amWidget, &Graph::slotSyncTracer);
}

void
AmAndPhGraph::setTracerVisible(bool isVisible)
{
  m_isVisible = isVisible;
  ui->amWidget->setTracerVisible(isVisible);
  ui->phWidget->setTracerVisible(isVisible);
}

void
AmAndPhGraph::autoScaleAxis()
{
  //调整图像
  ui->amWidget->rescaleAxes();
  ui->phWidget->rescaleAxes();
  ui->amWidget->replot();
  ui->phWidget->replot();
}

void
AmAndPhGraph::setTitle(const QString& title)
{
  ui->titleLabel->setText(title);
}

void
AmAndPhGraph::setAmplitudeTitle(const QString& xTitle, const QString& yTitle)
{
  ui->amWidget->xAxis->setLabel(xTitle);
  ui->amWidget->yAxis->setLabel(yTitle);
}

void
AmAndPhGraph::setPhaseTitle(const QString& xTitle, const QString& yTitle)
{
  ui->phWidget->xAxis->setLabel(xTitle);
  ui->phWidget->yAxis->setLabel(yTitle);
}

auto
AmAndPhGraph::doFFTAndShowResult(const DataVector& data) -> int
{
  if (data.isEmpty()) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("空数据"));
    return -1;
  }
  //为fft输入计算分配空间
  int N = data.size(); //采样点数
  double* in = (double*)fftw_malloc(sizeof(double) * N);
  if (in == nullptr) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("分配输入空间失败"));
    return -1;
  }
  int index = 0;
  Q_FOREACH (const auto& pt, data) {
    //数据写入缓冲区
    in[index] = pt.value;
    index++;
  }
  //为fft输出分配空间
  fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
  if (out == nullptr) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("分配输出空间失败"));
    return -1;
  }
  //进行fft变换，fftw_plan_dft_c2r_1d函数进行反变换
  //输入纯实数 real2complexs
  fftw_plan p = FFTW3_H::fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
  fftw_execute(p);
  //计算分辨率(频率间隔)
  //采样频率，由两个点的dx算出
  double fs = 1 / (data[1].key - data[0].key);
  double f0 = fs / N; //分辨率(频率间隔)f0
  // clear old data
  m_amplitude.clear();
  m_phase.clear();
  for (int i = 0; i < N / 2; i++) {
    //根据FFT计算的复数计算振幅谱
    double f = i * f0; //横坐标 频率HZ
    double Re = out[i][0];
    double Im = out[i][1];
    double Xk = qSqrt(qPow(Re, 2) + qPow(Im, 2)); //幅值
    double Xm = Xk * 2 / N;                       //幅值量化变换
    m_amplitude.append(QCPGraphData(f, Xm));
    //根据FFT计算的复数计算相位谱
    double phi = qAtan2(Im, Re);      //相位
    double phiDeg = phi * 180 / M_PI; //化成deg(°)位
    m_phase.append(QCPGraphData(f, phiDeg));
  }
  //直流信号的处理
  m_amplitude[0].value = m_amplitude[0].value / 2;
  //替换数据
  ui->amWidget->graph(0)->data()->set(m_amplitude, true);
  ui->phWidget->graph(0)->data()->set(m_phase, true);
  //调整图像
  ui->amWidget->rescaleAxes();
  ui->phWidget->rescaleAxes();
  //绘制bode图
  ui->amWidget->replot();
  ui->phWidget->replot();
  //释放buffer
  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
  return 0;
}

void
AmAndPhGraph::setDataAndShowBode(const DataVector& amplitude,
                                 const DataVector& phase)
{}

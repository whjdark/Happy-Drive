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
    ui->amWidget, &Graph::signalTracerMove, ui->phWidget, &Graph::slotSyncTracer);
  connect(
    ui->phWidget, &Graph::signalTracerMove, ui->amWidget, &Graph::slotSyncTracer);
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

void
AmAndPhGraph::showAmplitude(const DataVector& amplitude)
{
  //替换数据
  ui->amWidget->graph(0)->data()->set(amplitude, true);
  //调整图像
  ui->amWidget->rescaleAxes();
  //刷新graph
  ui->amWidget->replot();
}

void
AmAndPhGraph::showPhase(const DataVector& phase)
{
  ui->phWidget->graph(0)->data()->set(phase, true);
  ui->phWidget->rescaleAxes();
  ui->phWidget->replot();
}

void
AmAndPhGraph::showGraph(const DataVector& amplitude, const DataVector& phase)
{
  showAmplitude(amplitude);
  showPhase(phase);
}

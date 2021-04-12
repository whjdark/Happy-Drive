#include "oscilloscope.h"

Oscilloscope::Oscilloscope(QWidget* parent,
                           bool isXYTracer,
                           ScopeTraceLine::LineType lineType)
  : QCustomPlot(parent)
  , m_tracerVisible(false)
  , m_xTracer(nullptr)
  , m_yTracer(nullptr)
  , m_isXYTracer(isXYTracer)
  , m_scopeTracers()
  , m_line(nullptr)
  , m_lineType(lineType)
{}

void
Oscilloscope::initGraphTracer()
{
  //不知道为什么在初始化列表进行new就非常卡顿
  //所以在初始化图像完成后调用该函数初始化游标
  //这样就不卡了，原因不明？？？？
  // QCP没有在始化列表里面new的情况,可能和QCP的初始化设定有关
  if (m_isXYTracer) {
    m_xTracer = new ScopeTracer(this, this, ScopeTracer::XAxisTracer);
    m_yTracer = new ScopeTracer(this, this, ScopeTracer::YAxisTracer);
  }
  //创建新游标 == 曲线数量
  for (int i = 0; i < this->graphCount(); ++i) {
    auto* tracer = new ScopeTracer(this, this, ScopeTracer::GraphTracer);
    m_scopeTracers.append(tracer);
  }
  m_line = new ScopeTraceLine(this, this, m_lineType);
}

void
Oscilloscope::setTracerVisible(bool visible)
{
  m_tracerVisible = visible;
  if (m_isXYTracer) {
    m_xTracer->setVisible(m_tracerVisible);
    m_yTracer->setVisible(m_tracerVisible);
  }
  Q_FOREACH (ScopeTracer* tracer, m_scopeTracers) {
    tracer->setVisible(m_tracerVisible);
  }
  m_line->setVisible(m_tracerVisible);
  this->replot();
}

void
Oscilloscope::mouseMoveEvent(QMouseEvent* event)
{
  QCustomPlot::mouseMoveEvent(event);
  if (!m_tracerVisible) {
    return;
  }
  //当前鼠标位置（像素坐标）
  int x_pos = event->pos().x();
  int y_pos = event->pos().y();
  //像素坐标转成实际的x,y轴的坐标
  double x_val = this->xAxis->pixelToCoord(x_pos);
  double y_val = this->yAxis->pixelToCoord(y_pos);
  //更新xy轴游标坐标
  if (m_isXYTracer) {
    m_xTracer->updatePosition(x_val, y_val);
    m_yTracer->updatePosition(x_val, y_val);
  }
  //更新 GraphTracer
  for (int i = 0; i < this->graphCount(); ++i) {
    ScopeTracer* tracer = m_scopeTracers[i];
    if (graph(i)->visible()) { //游标与其对应的曲线同步显示
      tracer->setVisible(true);
      tracer->setPen(this->graph(i)->pen()); // 颜色与曲线一致
      tracer->setBrush(Qt::NoBrush);
      tracer->setLabelPen(this->graph(i)->pen());
      //游标与其对应的曲线颜色同步
      //根据鼠标的X找对最近曲线上的Y, true代表向左搜索
      double value = this->graph(i)->data()->findBegin(x_val, true)->value;
      tracer->updatePosition(x_val, value);
    } else {
      tracer->setVisible(false);
    }
  }
  //十字线跟更新
  m_line->updatePosition(x_val, y_val);
  //曲线重绘,使用绘制队列避免不必要的多次重绘
  this->replot(QCustomPlot::rpQueuedReplot);
}

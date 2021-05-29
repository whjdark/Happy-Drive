#include "graph.h"

Graph::Graph(QWidget* parent)
  : QCustomPlot(parent)
  , m_tracerVisible(true)
  , m_scopeTracer(nullptr)
  , m_line(nullptr)
{}

void
Graph::initGraphTracer()
{
  //同oscilloscope的init函数一样
  m_scopeTracer = new ScopeTracer(this, this, ScopeTracer::GraphTracer);
  m_line = new ScopeTraceLine(this, this, ScopeTraceLine::VerticalLine);
  m_line->setPen(QColor(Qt::black));
}

void
Graph::slotSyncTracer(double x)
{
  if (!m_tracerVisible) {
    return;
  }
  //更新 GraphTracer
  m_scopeTracer->setPen(QPen(QColor(Qt::blue)));
  m_scopeTracer->setBrush(Qt::NoBrush);
  m_scopeTracer->setLabelPen(QPen(QColor(Qt::blue)));
  //根据鼠标的X找对最近曲线上的Y, true代表向左搜索
  double value = this->graph(0)->data()->findBegin(x, true)->value;
  m_scopeTracer->updatePosition(x, value);
  //游标线更新
  //如果只画垂直线不需要y坐标
  m_line->updatePosition(x, 0);
  //曲线重绘,使用绘制队列避免不必要的多次重绘
  this->replot(QCustomPlot::rpQueuedReplot);
}

void
Graph::setTracerVisible(bool visible)
{
  m_tracerVisible = visible;
  m_scopeTracer->setVisible(m_tracerVisible);
  m_line->setVisible(m_tracerVisible);
  this->replot();
}

void
Graph::mouseMoveEvent(QMouseEvent* event)
{
  QCustomPlot::mouseMoveEvent(event);
  if (!m_tracerVisible) {
    return;
  }
  //当前鼠标位置（像素坐标）
  int x_pos = event->pos().x();
  //像素坐标转成实际的x轴的坐标
  double x_val = this->xAxis->pixelToCoord(x_pos);
  //更新 GraphTracer
  m_scopeTracer->setPen(QPen(QColor(Qt::blue)));
  m_scopeTracer->setBrush(Qt::NoBrush);
  m_scopeTracer->setLabelPen(QPen(QColor(Qt::blue)));
  //根据鼠标的X找对最近曲线上的Y, true代表向左搜索
  double value = this->graph(0)->data()->findBegin(x_val, true)->value;
  m_scopeTracer->updatePosition(x_val, value);
  //游标线更新
  //如果只画垂直线不需要y坐标
  m_line->updatePosition(x_val, 0);
  //曲线重绘,使用绘制队列避免不必要的多次重绘
  this->replot(QCustomPlot::rpQueuedReplot);
  emit signalTracerMove(x_val);
}

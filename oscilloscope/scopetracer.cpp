#include "scopetracer.h"

ScopeTracer::ScopeTracer(QObject* parent,
                         QCustomPlot* plot,
                         TracerType tracerType)
  : QObject(parent)
  , m_visible(true)
  , m_tracerType(tracerType)
  , m_plot(plot)
  , m_tracer(nullptr) // 跟踪的点
  , m_label(nullptr)  // 显示的数值
  , m_arrow(nullptr)  // 箭头
{
  if (!m_plot) {
    return;
  }
  // init pen
  QColor clrDefault(Qt::yellow);
  QBrush brushDefault(Qt::NoBrush);
  QPen penDefault(clrDefault);
  penDefault.setWidthF(0.5);
  // init tracer
  m_tracer = new QCPItemTracer(m_plot);
  m_tracer->setStyle(QCPItemTracer::tsCircle);
  m_tracer->setPen(penDefault);
  m_tracer->setBrush(brushDefault);
  // init text
  m_label = new QCPItemText(m_plot);
  m_label->setLayer("overlay");
  m_label->setClipToAxisRect(false);
  m_label->setPadding(QMargins(5, 5, 5, 5));
  m_label->setBrush(brushDefault);
  m_label->setPen(penDefault);
  m_label->position->setParentAnchor(m_tracer->position);
  m_label->setFont(QFont("Arial", 10));
  m_label->setColor(clrDefault);
  m_label->setText("");
  // init arrow
  m_arrow = new QCPItemLine(m_plot);
  QPen arrowPen(clrDefault, 1);
  m_arrow->setPen(penDefault);
  m_arrow->setLayer("overlay");
  m_arrow->setClipToAxisRect(false);
  m_arrow->setHead(QCPLineEnding::esSpikeArrow); //设置头部为箭头形状

  // set up tracer
  switch (m_tracerType) {
    case XAxisTracer: {
      m_tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
      m_tracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
      m_tracer->setSize(7);
      m_label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
      m_arrow->end->setParentAnchor(m_tracer->position);
      m_arrow->start->setParentAnchor(m_arrow->end);
      m_arrow->start->setCoords(0, 20); //偏移量
      break;
    }
    case YAxisTracer: {
      m_tracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
      m_tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
      m_tracer->setSize(7);
      m_label->setPositionAlignment(Qt::AlignRight | Qt::AlignHCenter);
      m_arrow->end->setParentAnchor(m_tracer->position);
      m_arrow->start->setParentAnchor(m_label->position);
      m_arrow->start->setCoords(-20, 0); //偏移量
      break;
    }
    case GraphTracer: {
      m_tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
      m_tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
      m_tracer->setSize(5);
      m_label->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      m_arrow->end->setParentAnchor(m_tracer->position);
      m_arrow->start->setParentAnchor(m_arrow->end);
      m_arrow->start->setCoords(20, 0); //偏移量
      break;
    }
    default:
      break;
  }
  setVisible(false);
}

void
ScopeTracer::setPen(const QPen& pen)

{
  if (m_tracer) {
    m_tracer->setPen(pen);
  }
  if (m_arrow) {
    m_arrow->setPen(pen);
  }
}

void
ScopeTracer::setBrush(const QBrush& brush)

{
  if (m_tracer) {
    m_tracer->setBrush(brush);
  }
}

void
ScopeTracer::setLabelPen(const QPen& pen)

{
  if (m_label) {
    m_label->setPen(pen);
    m_label->setBrush(Qt::NoBrush);
    m_label->setColor(pen.color());
  }
}

void
ScopeTracer::setText(const QString& text)

{
  if (m_label) {
    m_label->setText(text);
  }
}

void
ScopeTracer::setVisible(bool isVisible)

{
  m_visible = isVisible;
  if (m_tracer) {
    m_tracer->setVisible(m_visible);
  }
  if (m_label) {
    m_label->setVisible(m_visible);
  }
  if (m_arrow) {
    m_arrow->setVisible(m_visible);
  }
}

void
ScopeTracer::updatePosition(double xValue, double yValue)
{
  if (!m_visible) {
    return;
  }
  //防止Y跑出当前坐标轴
  if (yValue > m_plot->yAxis->range().upper) {
    yValue = m_plot->yAxis->range().upper;
  }
  switch (m_tracerType) {
    case XAxisTracer: {
      m_tracer->position->setCoords(xValue, 1);
      m_label->position->setCoords(0, 15);
      m_arrow->start->setCoords(0, 15);
      m_arrow->end->setCoords(0, 0);
      setText(QString::number(xValue, 'f', 2));
      break;
    }
    case YAxisTracer: {
      m_tracer->position->setCoords(0, yValue);
      m_label->position->setCoords(-20, 0);
      setText(QString::number(yValue, 'f', 2));
      break;
    }
    case GraphTracer: {
      m_tracer->position->setCoords(xValue, yValue);
      m_label->position->setCoords(20, 0);
      QString xStr = QString::number(xValue, 'f', 2);
      QString yStr = QString::number(yValue, 'f', 2);
      setText(tr("x:%1\ny:%2").arg(xStr).arg(yStr));
      break;
    }
    default:
      break;
  }
}

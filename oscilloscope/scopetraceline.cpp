#include "scopetraceline.h"

ScopeTraceLine::ScopeTraceLine(QObject* parent,
                               QCustomPlot* plot,
                               LineType lineType)
  : QObject(parent)
  , m_plot(plot)
  , m_lineType(lineType)
  , m_lineV(nullptr)
  , m_lineH(nullptr)
{
  initLine();
}

void
ScopeTraceLine::initLine()
{
  if (!m_plot) {
    return;
  }
  //初始化垂直线或水平线或都初始化
  QPen linesPen(Qt::green, 1, Qt::DashLine);
  if (m_lineType == VerticalLine || m_lineType == Both) {
    m_lineV = new QCPItemStraightLine(m_plot); //垂直线
    m_lineV->setLayer("overlay");              //覆盖图像
    m_lineV->setPen(linesPen);
    m_lineV->setClipToAxisRect(true); //贴在轴线上
    m_lineV->point1->setCoords(0, 0);
    m_lineV->point2->setCoords(0, 0);
  }
  if (m_lineType == HorizonLine || m_lineType == Both) {
    m_lineH = new QCPItemStraightLine(m_plot); //水平线
    m_lineH->setLayer("overlay");
    m_lineH->setPen(linesPen);
    m_lineH->setClipToAxisRect(true);
    m_lineH->point1->setCoords(0, 0);
    m_lineH->point2->setCoords(0, 0);
  }
}

void
ScopeTraceLine::setVisible(bool isVisible)
{
  m_visible = isVisible;
  if (m_lineV) {
    m_lineV->setVisible(isVisible);
  }
  if (m_lineH) {
    m_lineH->setVisible(isVisible);
  }
}

void
ScopeTraceLine::setPen(const QColor& color)
{
  QPen linesPen(color, 1, Qt::DashLine);
  if (m_lineType == VerticalLine || m_lineType == Both) {
    m_lineV->setPen(linesPen);
  }
  if (m_lineType == HorizonLine || m_lineType == Both) {
    m_lineH->setPen(linesPen);
  }
}

void
ScopeTraceLine::updatePosition(double xValue, double yValue)
{
  if (!m_visible) {
    return;
  }
  //两条线画在坐标轴范围之内
  if (m_lineType == VerticalLine || m_lineType == Both) {
    if (m_lineV) {
      m_lineV->point1->setCoords(xValue, m_plot->yAxis->range().lower);
      m_lineV->point2->setCoords(xValue, m_plot->yAxis->range().upper);
    }
  }
  if (m_lineType == HorizonLine || m_lineType == Both) {
    if (m_lineH) {
      m_lineH->point1->setCoords(m_plot->xAxis->range().lower, yValue);
      m_lineH->point2->setCoords(m_plot->xAxis->range().upper, yValue);
    }
  }
}

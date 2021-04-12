#ifndef SCOPETRACELINE_H
#define SCOPETRACELINE_H

#include <QObject>

#include "../QCustomPlot/qcustomplot.h"

class ScopeTraceLine : public QObject
{
  Q_OBJECT

public:
  enum LineType
  {
    VerticalLine, //垂直线
    HorizonLine,  //水平线
    Both          //同时显示水平和垂直线
  };
  Q_ENUM(LineType);

public:
  explicit ScopeTraceLine(QObject* parent = nullptr,
                          QCustomPlot* plot = nullptr,
                          LineType lineType = Both);
  ~ScopeTraceLine() = default;

  void initLine();
  void updatePosition(double xValue, double yValue);
  void setVisible(bool isVisible);
  void setPen(const QColor& color);

private:
  bool m_visible;               //是否可见
  QCustomPlot* m_plot;          //图表
  LineType m_lineType;          //线形
  QCPItemStraightLine* m_lineV; //垂直线
  QCPItemStraightLine* m_lineH; //水平线
};

#endif // SCOPETRACELINE_H

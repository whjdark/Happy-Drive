#ifndef SCOPETRACER_H
#define SCOPETRACER_H

#include <QObject>

#include "../QCustomPlot/qcustomplot.h"

class ScopeTracer : public QObject

{
  Q_OBJECT
public:
  enum TracerType
  {
    XAxisTracer, //依附在x轴上显示x值
    YAxisTracer, //依附在y轴上显示y值
    GraphTracer  //在图中显示x,y值
  };
  Q_ENUM(TracerType);

public:
  explicit ScopeTracer(QObject* parent = nullptr,
                       QCustomPlot* plot = nullptr,
                       TracerType tracerType = GraphTracer);
  ~ScopeTracer() = default;

  void setPen(const QPen& pen);
  void setBrush(const QBrush& brush);
  void setText(const QString& text);
  void setLabelPen(const QPen& pen);
  void setVisible(bool isVisible);
  void updatePosition(double xValue, double yValue);

private:
  bool m_visible;          //是否可见
  TracerType m_tracerType; //类型
  QCustomPlot* m_plot;     //图表
  QCPItemTracer* m_tracer; //跟踪的点
  QCPItemText* m_label;    //显示的数值
  QCPItemLine* m_arrow;    //箭头
};

#endif // SCOPETRACER_H

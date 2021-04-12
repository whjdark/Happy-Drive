#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include <QObject>

#include "scopetraceline.h"
#include "scopetracer.h"

class Oscilloscope : public QCustomPlot
{
  Q_OBJECT

public:
  explicit Oscilloscope(
    QWidget* parent = nullptr,
    bool isXYTracer = true,
    ScopeTraceLine::LineType lineType = ScopeTraceLine::Both);
  ~Oscilloscope() = default;

  void setTracerVisible(bool visible);
  void initGraphTracer();

protected:
  virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
  bool m_tracerVisible; //是否显示追踪器（鼠标在图中移动，显示对应的值）
  ScopeTracer* m_xTracer;               // x轴游标
  ScopeTracer* m_yTracer;               // y轴游标
  bool m_isXYTracer;                    // 是否显示xy轴游标
  QVector<ScopeTracer*> m_scopeTracers; //图形内游标
  ScopeTraceLine* m_line;               //游标线
  ScopeTraceLine::LineType m_lineType;  //游标线类型
};

#endif // OSCILLOSCOPE_H

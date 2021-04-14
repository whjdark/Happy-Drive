#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>

#include "../oscilloscope/scopetraceline.h"
#include "../oscilloscope/scopetracer.h"

class Graph : public QCustomPlot
{
  Q_OBJECT

public:
  explicit Graph(QWidget* parent = nullptr);
  ~Graph() = default;

  void setTracerVisible(bool visible);
  void initGraphTracer();

  //以下两个函数用于多个图像之间游标同步移动
Q_SIGNALS:
  void tracerMove(double x);

public Q_SLOTS:
  void slotSyncTracer(double x);

protected:
  virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
  bool m_tracerVisible; //是否显示追踪器（鼠标在图中移动，显示对应的值）
  ScopeTracer* m_scopeTracer; //图形内游标
  ScopeTraceLine* m_line;     //游标线
};

#endif // GRAPH_H

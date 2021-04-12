#ifndef STATISTICSWINDOW_H
#define STATISTICSWINDOW_H

#include <QMainWindow>

#include "../../QCustomPlot/qcustomplot.h"

namespace Ui {
class StatisticsWindow;
}

class StatisticsWindow : public QMainWindow
{
  Q_OBJECT

public:
  enum TableCols
  {
    MAX_COL = 0, //"最大值"
    MIN_COL = 1, //"最小值"
    AVG_COL = 2, // "平均值"
    VPP_COL = 3, //"峰峰值"
    RMS_COL = 4, //"有效值/均方根值"
  };
  Q_ENUM(TableCols)

  //每个通道的点数据类型
  //支持多个通道，要求与datatracer的maxChannels大小一致
  typedef std::array<QVector<QCPGraphData>, 8> PointsData;

public:
  explicit StatisticsWindow(QWidget* parent = nullptr);
  ~StatisticsWindow();

  void setRowHeaderText(int row, const QString& title);
  void setRowTextColor(int row, const QColor& color);
  void updateStatistics(const PointsData& points);

private:
  Ui::StatisticsWindow* ui;
};

#endif // STATISTICSWINDOW_H

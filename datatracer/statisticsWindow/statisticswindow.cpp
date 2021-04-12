#include "statisticswindow.h"
#include "ui_statisticswindow.h"

StatisticsWindow::StatisticsWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::StatisticsWindow)
{
  ui->setupUi(this);
}

StatisticsWindow::~StatisticsWindow()
{
  delete ui;
}

void
StatisticsWindow::setRowHeaderText(int row, const QString& title)
{
  //设置表头标题
  QTableWidgetItem* headerItem = ui->tableWidget->verticalHeaderItem(row);
  headerItem->setText(title);
}

void
StatisticsWindow::setRowTextColor(int row, const QColor& color)
{
  //设置表头标题的颜色
  QTableWidgetItem* headerItem = ui->tableWidget->verticalHeaderItem(row);
  headerItem->setForeground(QBrush(color));
  //设置单元格的颜色
  for (int i = 0; i < ui->tableWidget->columnCount(); i++) {
    //对一行的所有列进行修改
    QTableWidgetItem* cellItem = ui->tableWidget->item(row, i);
    // 设置前背景，也就是字体颜色
    cellItem->setForeground(QBrush(color));
    // 设置字体中间对齐
    cellItem->setTextAlignment(Qt::AlignCenter);
  }
}

void
StatisticsWindow::updateStatistics(const PointsData& points)
{
  // table的row count与通道index对应
  for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
    if (points[i].isEmpty()) {
      //如果该通道没数据
      QTableWidgetItem* maxValItem = ui->tableWidget->item(i, MAX_COL);
      maxValItem->setText(QStringLiteral("-"));
      QTableWidgetItem* minValItem = ui->tableWidget->item(i, MIN_COL);
      minValItem->setText(QStringLiteral("-"));
      QTableWidgetItem* avgValItem = ui->tableWidget->item(i, AVG_COL);
      avgValItem->setText(QStringLiteral("-"));
      QTableWidgetItem* vppValItem = ui->tableWidget->item(i, VPP_COL);
      vppValItem->setText(QStringLiteral("-"));
      QTableWidgetItem* rmsValItem = ui->tableWidget->item(i, RMS_COL);
      rmsValItem->setText(QStringLiteral("-"));
      continue;
    }
    //计算该通道最大最小值 平均值
    double max = INT_MIN;
    double min = INT_MAX;
    double total = 0;
    double squreTotal = 0;
    int count = 0;
    Q_FOREACH (const auto& pt, points[i]) {
      if (pt.mainValue() > max) {
        max = pt.mainValue();
      }
      if (pt.mainValue() < min) {
        min = pt.mainValue();
      }
      total += pt.mainValue();
      squreTotal += qPow(pt.mainValue(), 2);
      count++;
    }
    double rms = qSqrt(squreTotal / count);
    QTableWidgetItem* maxValItem = ui->tableWidget->item(i, MAX_COL);
    maxValItem->setText(QString::number(max, 'f', 2));
    QTableWidgetItem* minValItem = ui->tableWidget->item(i, MIN_COL);
    minValItem->setText(QString::number(min, 'f', 2));
    QTableWidgetItem* avgValItem = ui->tableWidget->item(i, AVG_COL);
    avgValItem->setText(QString::number(total / count, 'f', 2));
    QTableWidgetItem* vppValItem = ui->tableWidget->item(i, VPP_COL);
    vppValItem->setText(QString::number(max - min, 'f', 2));
    QTableWidgetItem* rmsValItem = ui->tableWidget->item(i, RMS_COL);
    rmsValItem->setText(QString::number(rms, 'f', 2));
  }
}

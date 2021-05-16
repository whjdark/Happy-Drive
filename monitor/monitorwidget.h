/*
 * @Author: whj
 * @Date: 2021-02-04 22:07:13
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:04:37
 * @Description: file content
 */

#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QMessageBox>
#include <QTimer>
#include <QWidget>

#include "../BaseDataType/driverdatatype.h"
#include "../xcomm/xcomm.h"

namespace Ui {
class MonitorWidget;
}

class MonitorWidget : public QWidget
{
  Q_OBJECT

public:
  enum TableCols
  {
    NUM_COL = 0,    //"编号"
    CODE_COL = 1,   //"代号"
    NAME_COL = 2,   //"名称"
    VAL_COL = 3,    // "当前值"
    UNIT_COL = 4,   // "单位"
    DETAIL_COL = 5, //"详细";
  };
  Q_ENUM(TableCols)

  enum TableRows
  {
    FAULTCOME_ROW = 0, //"错误码"
  };
  Q_ENUM(TableRows)

public:
  explicit MonitorWidget(QWidget* parent = nullptr, XComm* xcomm = nullptr);
  ~MonitorWidget();

  void startMonitor();
  void stopMonitor();

private Q_SLOTS:
  void slotUpdateTable();
  void slotProccessCmd(const quint16 cmd, const QByteArray& data);
  void on_startButton_clicked();
  void on_stopButton_clicked();

private:
  void initPanel();
  void initConnetions();
  void initTableHeader();
  void initTableData();
  template<typename T>
  void createItemsARow(const int row,
                       const QString& num,
                       const QString& code,
                       const QString& name,
                       const T value,
                       const bool isHex,
                       const QString& unit,
                       const QString& detail);
  template<typename T>
  void setTableData(const TableRows row,
                    const TableCols,
                    const T data,
                    const bool isHex);
  void updateTable();

private:
  Q_CONSTEXPR static int dataTableRow = 1;
  Q_CONSTEXPR static int refreshTime = 1000;

  Ui::MonitorWidget* ui;
  XComm* m_xcomm;
  DriverDataType::MonitorDataType m_monitorData;
  // timers
  QTimer* m_updateTimer;
};

#endif // MONITORWIDGET_H

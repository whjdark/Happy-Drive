/*
 * @Author: whj
 * @Date: 2021-03-08 00:35:34
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:03:21
 * @Description: file content
 */

#ifndef CONCONFIGWIDGET_H
#define CONCONFIGWIDGET_H

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QWidget>

#include "../BaseDataType/driverdatatype.h"
#include "../xcomm/xcomm.h"

namespace Ui {
class ConfigWidget;
}

class CONConfigWidget : public QWidget
{
  Q_OBJECT

public:
  enum TableCols
  {
    NUM_COL = 0,     //"参数编号"
    NAME_COL = 1,    //"参数名称"
    VAL_COL = 2,     // "参数值"
    UNIT_COL = 3,    // "单位"
    RANGE_COL = 4,   //"范围i"
    DEFAULT_COL = 5, //"默认值"
    DETAIL_COL = 6,  //"详细"
  };
  Q_ENUM(TableCols)

  enum RunMode
  {
    MODE0 = 0,
    MODE1 = 1,
    MODE2 = 2,
    MODE3 = 3,
    MODE4 = 4,
    MODE5 = 5,
    MODE6 = 6,

  };
  Q_ENUM(RunMode)

  struct ParamsRow
  {
    QString num;
    QString name;
    double value;
    bool isHex;
    QString unit;
    QString range;
    QString defaultValue;
    QString detail;
  };

public:
  explicit CONConfigWidget(QWidget* parent = nullptr, XComm* xcomm = nullptr);
  ~CONConfigWidget();

  void resetDSPRunMode();
  QString currentDSPRunMode() const;

private Q_SLOTS:
  void on_writeDSPButon_clicked();
  void on_loadJSONButton_clicked();
  void on_saveJSONButton_clicked();
  void on_cleartTableButton_clicked();

  void on_runModeBox_currentIndexChanged(int index);

  void slotProcessCmd(const quint16 cmd, const QByteArray& data);

private:
  void initTableHeader();
  void initTableData();
  void initMode0Tables();
  void initMode1Tables();
  void initMode2Tables();
  void initMode3Tables();
  void initMode4Tables();
  void initMode5Tables();
  void initMode6Tables();
  void initConnetions();
  double inputCheck(const int row, bool* isOk);

  template<typename T>
  void createItemsARow(const int row,
                       const QString& num,
                       const QString& name,
                       const T value,
                       const bool isHex,
                       const QString& unit,
                       const QString& range,
                       const QString& defaultValue,
                       const QString& detail);
  template<typename T>
  void setTableData(const int row,
                    const CONConfigWidget::TableCols col,
                    const T data,
                    const bool isHex);

  void refreshModeTable(CONConfigWidget::RunMode);
  void resetTableValue();
  void checkDSPConfig(const QByteArray& data);

private:
  //支持的模式
  Q_CONSTEXPR static int numOfMode = 7;
  //参数配置最大容忍误差
  Q_CONSTEXPR static double tolerantConfigError = 0.1;

  Ui::ConfigWidget* ui;
  XComm* m_xcomm;
  std::array<QVector<ParamsRow>, numOfMode> m_tableConfig;
  //上一个表格的mode
  RunMode m_lastTableMode = MODE0;
  //成功写入到DSP的模式, 也就是正在使用的模式
  RunMode m_currentDSPRunMode = MODE0;
};

#endif // CONCONFIGWIDGET_H

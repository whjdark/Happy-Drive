/*
 * @Author: whj
 * @Date: 2021-03-08 00:35:34
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:03:10
 * @Description: file content
 */

#include "CONconfigwidget.h"

#include "ui_CONconfigwidget.h"

CONConfigWidget::CONConfigWidget(QWidget* parent, XComm* xcomm)
  : QWidget(parent)
  , ui(new Ui::ConfigWidget)
  , m_xcomm(xcomm)
{
  ui->setupUi(this);

  // initPanel();
  initTableHeader();
  initTableData();
  // init mode table content
  initMode0Tables();
  initMode1Tables();
  initMode2Tables();
  initMode3Tables();
  initMode4Tables();
  initMode5Tables();
  initMode6Tables();
  initConnetions();
}

CONConfigWidget::~CONConfigWidget()
{
  delete ui;
}

void
CONConfigWidget::initTableHeader()
{
  //设置标题
  //表头标题用QStringList来表示
  ui->tableWidget->setWindowTitle(QStringLiteral("参数配置"));
  const static QStringList headerText{ "参数编号", "参数名称", "设定值", "单位",
                                       "范围",     "默认值",   "详细" };
  //列数设置为与 headerText的行数相等
  ui->tableWidget->setColumnCount(headerText.count());
  //填充表头
  ui->tableWidget->setHorizontalHeaderLabels(headerText);
  //调整表头
  for (int i = 0; i < ui->tableWidget->columnCount(); i++) {
    QTableWidgetItem* headerItem = ui->tableWidget->horizontalHeaderItem(i);
    //设置字体颜色大小
    QFont font = headerItem->font();
    font.setBold(true);
    font.setPointSize(10);
    headerItem->setForeground(Qt::darkGreen);
    headerItem->setFont(font);
  }
  //调整列宽以适应单元内容
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(
    QHeaderView::ResizeToContents);
}

template<typename T>
void
CONConfigWidget::createItemsARow(const int row,
                                 const QString& num,
                                 const QString& name,
                                 const T value,
                                 const bool isHex,
                                 const QString& unit,
                                 const QString& range,
                                 const QString& defaultValue,
                                 const QString& detail)
{
  //编号
  auto* numItem = new QTableWidgetItem(num);
  numItem->setFlags(Qt::ItemIsUserCheckable); //不可编辑
  ui->tableWidget->setItem(row, NUM_COL, numItem);
  //名称
  auto* nameItem = new QTableWidgetItem(name);
  nameItem->setFlags(Qt::ItemIsUserCheckable); //不可编辑
  ui->tableWidget->setItem(row, NAME_COL, nameItem);
  //当前值
  auto* vaueItem = new QTableWidgetItem(); //可编辑
  if (isHex) {
    vaueItem->setText(tr("0x%1").arg(qulonglong(value), 0, 16));
  } else { // !isHex
    vaueItem->setText(tr("%1").arg(value, 0));
  }
  ui->tableWidget->setItem(row, VAL_COL, vaueItem);
  //单位
  auto* unitItem = new QTableWidgetItem(unit);
  unitItem->setFlags(Qt::ItemIsUserCheckable); //不可编辑
  ui->tableWidget->setItem(row, UNIT_COL, unitItem);
  //范围
  auto* rangeItem = new QTableWidgetItem(range);
  rangeItem->setFlags(Qt::ItemIsUserCheckable); //不可编辑
  ui->tableWidget->setItem(row, RANGE_COL, rangeItem);
  //默认值
  auto* defaultValueItem = new QTableWidgetItem(defaultValue);
  defaultValueItem->setFlags(Qt::ItemIsUserCheckable); //不可编辑
  ui->tableWidget->setItem(row, DEFAULT_COL, defaultValueItem);
  //详细
  auto* detailItem = new QTableWidgetItem(detail);
  detailItem->setFlags(Qt::ItemIsUserCheckable); //不可编辑
  ui->tableWidget->setItem(row, DETAIL_COL, detailItem);
}

template<typename T>
void
CONConfigWidget::setTableData(const int row,
                              const CONConfigWidget::TableCols col,
                              const T data,
                              const bool isHex)
{
  QTableWidgetItem* item = ui->tableWidget->item(row, col);
  if (isHex) {
    item->setText(tr("0x%1").arg(qulonglong(data), 0, 16));
  } else { // !isHex
    item->setText(tr("%1").arg(data, 0));
  }
}

void
CONConfigWidget::initTableData()
{
  //奇数偶数行颜色交替
  ui->tableWidget->setAlternatingRowColors(true);
  //最后一行拉伸填充
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  //选中时一行整体选中
  ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  //初始化表格数据
  // 默认为mode0 无参数设置
  ui->tableWidget->setRowCount(0);
}

void
CONConfigWidget::initMode0Tables()
{
  //模式0菜单参数配置表
  // mode0, nothing
}

void
CONConfigWidget::initMode1Tables()
{
  //模式1菜单参数配置表
  ParamsRow paramsRow;
  // 电流pid kp
  paramsRow.num = QStringLiteral("0x0");
  paramsRow.name = QStringLiteral("电流PID-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE1].append(paramsRow);
  // 电流pid ki
  paramsRow.num = QStringLiteral("0x1");
  paramsRow.name = QStringLiteral("电流PID-ki");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE1].append(paramsRow);
  // 电流pid kc
  paramsRow.num = QStringLiteral("0x17");
  paramsRow.name = QStringLiteral("电流PID-kc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE1].append(paramsRow);
  // 速度PID-kp
  paramsRow.num = QStringLiteral("0x3");
  paramsRow.name = QStringLiteral("速度PID-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE1].append(paramsRow);
  // 速度PID-ki
  paramsRow.num = QStringLiteral("0x4");
  paramsRow.name = QStringLiteral("速度PID-ki");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE1].append(paramsRow);
  // 速度PID-kc
  paramsRow.num = QStringLiteral("0x18");
  paramsRow.name = QStringLiteral("速度PID-kc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE1].append(paramsRow);
}

void
CONConfigWidget::initMode2Tables()
{
  //模式2菜单参数配置表
  ParamsRow paramsRow;
  //电流PID-kp
  paramsRow.num = QStringLiteral("0x0");
  paramsRow.name = QStringLiteral("电流PID-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE2].append(paramsRow);
  // 电流pid ki
  paramsRow.num = QStringLiteral("0x1");
  paramsRow.name = QStringLiteral("电流PID-ki");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE2].append(paramsRow);
  // 电流pid kc
  paramsRow.num = QStringLiteral("0x17");
  paramsRow.name = QStringLiteral("电流PID-kc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE2].append(paramsRow);
  // 速度PID-kp
  paramsRow.num = QStringLiteral("0x3");
  paramsRow.name = QStringLiteral("速度PID-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE2].append(paramsRow);
  // 速度PID-ki
  paramsRow.num = QStringLiteral("0x4");
  paramsRow.name = QStringLiteral("速度PID-ki");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE2].append(paramsRow);
  // 速度PID-kc
  paramsRow.num = QStringLiteral("0x18");
  paramsRow.name = QStringLiteral("速度PID-kc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE2].append(paramsRow);
  // 位置PID-kp
  paramsRow.num = QStringLiteral("0x6");
  paramsRow.name = QStringLiteral("位置PID-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE2].append(paramsRow);
}

void
CONConfigWidget::initMode3Tables()
{
  //模式3菜单参数配置表
  ParamsRow paramsRow;
  //电流PID-kp
  paramsRow.num = QStringLiteral("0x0");
  paramsRow.name = QStringLiteral("电流PID-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE3].append(paramsRow);
  // 电流pid ki
  paramsRow.num = QStringLiteral("0x1");
  paramsRow.name = QStringLiteral("电流PID-ki");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE3].append(paramsRow);
  // 电流pid kc
  paramsRow.num = QStringLiteral("0x17");
  paramsRow.name = QStringLiteral("电流PID-kc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE3].append(paramsRow);
  // 速度ADRC-wo
  paramsRow.num = QStringLiteral("0xC");
  paramsRow.name = QStringLiteral("速度ADRC-wo");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE3].append(paramsRow);
  // 速度ADRC-wc
  paramsRow.num = QStringLiteral("0xD");
  paramsRow.name = QStringLiteral("速度ADRC-wc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE3].append(paramsRow);
  // 速度ADRC-bi
  paramsRow.num = QStringLiteral("0xE");
  paramsRow.name = QStringLiteral("速度ADRC-bi");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE3].append(paramsRow);
}

void
CONConfigWidget::initMode4Tables()
{
  //模式4菜单参数配置表
  ParamsRow paramsRow;
  //电流ADRC-woi
  paramsRow.num = QStringLiteral("0x9");
  paramsRow.name = QStringLiteral("电流ADRC-woi");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE4].append(paramsRow);
  // 电流ADRC-wci
  paramsRow.num = QStringLiteral("0xA");
  paramsRow.name = QStringLiteral("电流ADRC-wci");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE4].append(paramsRow);
  // 电流ADRC-bi
  paramsRow.num = QStringLiteral("0xB");
  paramsRow.name = QStringLiteral("电流ADRC-bi");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE4].append(paramsRow);
  // 速度ADRC-wo
  paramsRow.num = QStringLiteral("0xC");
  paramsRow.name = QStringLiteral("速度ADRC-wo");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE4].append(paramsRow);
  // 速度ADRC-wc
  paramsRow.num = QStringLiteral("0xD");
  paramsRow.name = QStringLiteral("速度ADRC-wc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE4].append(paramsRow);
  // 速度ADRC-bi
  paramsRow.num = QStringLiteral("0xE");
  paramsRow.name = QStringLiteral("速度ADRC-bi");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE4].append(paramsRow);
}

void
CONConfigWidget::initMode5Tables()
{
  //模式5菜单参数配置表
  ParamsRow paramsRow;
  //位置PID-kp
  paramsRow.num = QStringLiteral("0x6");
  paramsRow.name = QStringLiteral("位置PID-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-128");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE5].append(paramsRow);
  //电流ADRC-woi
  paramsRow.num = QStringLiteral("0x9");
  paramsRow.name = QStringLiteral("电流ADRC-woi");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE5].append(paramsRow);
  // 电流ADRC-wci
  paramsRow.num = QStringLiteral("0xA");
  paramsRow.name = QStringLiteral("电流ADRC-wci");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE5].append(paramsRow);
  // 电流ADRC-bi
  paramsRow.num = QStringLiteral("0xB");
  paramsRow.name = QStringLiteral("电流ADRC-bi");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE5].append(paramsRow);
  // 速度ADRC-wo
  paramsRow.num = QStringLiteral("0xC");
  paramsRow.name = QStringLiteral("速度ADRC-wo");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE5].append(paramsRow);
  // 速度ADRC-wc
  paramsRow.num = QStringLiteral("0xD");
  paramsRow.name = QStringLiteral("速度ADRC-wc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE5].append(paramsRow);
  // 速度ADRC-bi
  paramsRow.num = QStringLiteral("0xE");
  paramsRow.name = QStringLiteral("速度ADRC-bi");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE5].append(paramsRow);
}

void
CONConfigWidget::initMode6Tables()
{
  //模式6菜单参数配置表
  ParamsRow paramsRow;
  //电流PID-kp
  paramsRow.num = QStringLiteral("0x0");
  paramsRow.name = QStringLiteral("电流PID-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 电流pid ki
  paramsRow.num = QStringLiteral("0x1");
  paramsRow.name = QStringLiteral("电流PID-ki");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 电流pid kc
  paramsRow.num = QStringLiteral("0x17");
  paramsRow.name = QStringLiteral("电流PID-kc");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 速度FOPD-GESO kp
  paramsRow.num = QStringLiteral("0xF");
  paramsRow.name = QStringLiteral("速度FOPD-GESO-kp");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 速度FOPD-GESO kd
  paramsRow.num = QStringLiteral("0x10");
  paramsRow.name = QStringLiteral("速度FOPD-GESO-kd");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 速度FOPD-GESO mu
  paramsRow.num = QStringLiteral("0x11");
  paramsRow.name = QStringLiteral("速度FOPD-GESO-mu");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 速度FOPD-GESO omegaO
  paramsRow.num = QStringLiteral("0x12");
  paramsRow.name = QStringLiteral("速度FOPD-GESO-omegaO");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 速度FOPD-GESO b0
  paramsRow.num = QStringLiteral("0x13");
  paramsRow.name = QStringLiteral("速度FOPD-GESO-b0");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 速度FOPD-GESO a1
  paramsRow.num = QStringLiteral("0x14");
  paramsRow.name = QStringLiteral("速度FOPD-GESO-a1");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  // 速度FOPD-GESO a2
  paramsRow.num = QStringLiteral("0x15");
  paramsRow.name = QStringLiteral("速度FOPD-GESO-a2");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
  paramsRow.num = QStringLiteral("0x16");
  paramsRow.name = QStringLiteral("速度FOPD-GESO-h");
  paramsRow.value = 0.0;
  paramsRow.isHex = false;
  paramsRow.unit = QStringLiteral("");
  paramsRow.range = QStringLiteral("0-16384");
  paramsRow.defaultValue = QStringLiteral("0");
  paramsRow.detail = QStringLiteral("reserve");
  m_tableConfig[DriverDataType::MODE6].append(paramsRow);
}

void
CONConfigWidget::initConnetions()
{
  connect(
    m_xcomm, &XComm::signalConfigerCmd, this, &CONConfigWidget::slotProcessCmd);
}

void
CONConfigWidget::slotProcessCmd(const quint16 cmd, const QByteArray& data)
{
  // TBC
  switch (cmd) {
    case XComm::CONTROLLER_WRITE:
      // write config on DSP
      // 写入驱动器后从驱动器读取数据进行对比
      //检查是配置的数值是否在误差范围之内
      m_xcomm->command(XComm::CONTROLLER_READ, QByteArray());
      break;
    case XComm::CONTROLLER_READ:
      // check config from DSP
      checkDSPConfig(data);
      break;
    default:
      break;
  }
}

void
CONConfigWidget::on_writeDSPButon_clicked()
{
  if (m_xcomm->getConnectStatus() == XComm::COMM_IDLE) {
    // 未连接
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("未连接驱动器"));
    return;
  }
  if (m_xcomm->getMotorStatus() == XComm::MOTOR_RUN) {
    // 正在运行
    QMessageBox::warning(this,
                         QStringLiteral("错误"),
                         QStringLiteral("电机正在运行，不能配置参数"));
    return;
  }
  // 控制器数据结构体类型
  DriverDataType::CONDataType configData;
  //  read runMode from Box
  configData.data().runMode = ui->runModeBox->currentIndex();
  bool isOk = false;
  for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
    //根据参数编号读取表格数据传到POD结构体中发送
    int numOfParam =
      ui->tableWidget->item(row, NUM_COL)->text().toInt(nullptr, 16);
    //值
    double val = inputCheck(row, &isOk); // check input validation
    if (!isOk) {
      return;
    }
    //结构体下标0是运行模式
    //所以参数编号+1对应结构体下标
    // upscale for double, convert double to int
    double scaledVal = val * DriverDataType::_IQ15;
    // read table data and convert it to POD struct
    configData.at(numOfParam + 1) = static_cast<qint32>(scaledVal);
  }
  // send config command
  m_xcomm->command(XComm::CONTROLLER_WRITE, configData.toByteArray());
}

void
CONConfigWidget::on_loadJSONButton_clicked()
{
  QString fileName =
    QFileDialog::getOpenFileName(this,
                                 QStringLiteral("载入参数档案"),
                                 qApp->applicationDirPath(),
                                 QStringLiteral("JSON (*.json )"));
  if (fileName.isEmpty()) {
    return;
  }
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("打开文件失败！"));
    return;
  }
  //读取文件数据
  QByteArray saveParams = file.readAll();
  file.close();
  if (saveParams.isEmpty()) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("空档案！"));
    return;
  }
  //解析JSON数据
  QJsonParseError parseJsonErr = QJsonParseError();
  QJsonDocument loadDoc = QJsonDocument::fromJson(saveParams, &parseJsonErr);
  if (!(parseJsonErr.error == QJsonParseError::NoError)) {
    QMessageBox::warning(
      this,
      QStringLiteral("错误"),
      tr("解析json文件错误(ERROR: %1)！").arg(parseJsonErr.errorString()));
    return;
  }
  //读取JSON数据
  QJsonObject fileObject = loadDoc.object();
  //读取文件头
  if (!fileObject.contains(QStringLiteral("FileInfo"))) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("丢失文件头字段！"));
    return;
  }
  QJsonObject fileInfoObject =
    fileObject[QStringLiteral("FileInfo")].toObject();
  //读取运行模式数据
  if (!fileInfoObject.contains(QStringLiteral("ModeID"))) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("丢失ModeID字段！"));
    return;
  }
  int modeID = fileInfoObject[QStringLiteral("ModeID")].toInt();
  //检查运行模式合法性
  if (modeID < 0 || modeID > numOfMode) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), tr("不支持的模式(MODE%1)！").arg(modeID));
    return;
  }
  if (modeID == 0) {
    QMessageBox::warning(this, QStringLiteral("提示"), tr("模式0无参数"));
    return;
  }
  //设置到该运行模式的table
  ui->runModeBox->setCurrentIndex(modeID);
  //读取参数档案
  if (!fileObject.contains(QStringLiteral("Parameters"))) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("丢失Parameters字段！"));
    return;
  }
  QJsonObject paramsObject =
    fileObject[QStringLiteral("Parameters")].toObject();
  if (paramsObject.isEmpty()) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("空Parameters档案！"));
    return;
  }
  //检查JSON参数数量是否等于参数表格的的参数数量
  //如不等于，则可能有问题
  int rowCount = ui->tableWidget->rowCount();
  if (paramsObject.size() != rowCount) {
    QMessageBox::warning(
      this,
      QStringLiteral("错误"),
      tr("JSON参数数量与运行模式的参数数量不匹配(%1 != %2)！")
        .arg(paramsObject.size())
        .arg(rowCount));
    return;
  }
  //遍历当前参数表格，从JSON寻找并读入参数
  for (int row = 0; row < rowCount; row++) {
    // 读入一行参数
    QString name = ui->tableWidget->item(row, NAME_COL)->text();
    if (!paramsObject.contains(name)) {
      QMessageBox::warning(
        this, QStringLiteral("错误"), tr("丢失数据(%1)！").arg(name));
      //清空表格
      resetTableValue();
      return;
    }
    // data是否合法
    if (!paramsObject[name].isDouble()) {
      QMessageBox::warning(
        this,
        QStringLiteral("错误"),
        tr("非法的数据(%1:%2)！").arg(name, paramsObject[name].toString()));
      //清空表格
      resetTableValue();
      return;
    }
    // valid data
    double val = paramsObject[name].toDouble();
    setTableData(row, VAL_COL, val, false);
  }
  QMessageBox::warning(
    this, QStringLiteral("提示"), QStringLiteral("载入档案成功"));
}

void
CONConfigWidget::on_saveJSONButton_clicked()
{
  //保存JSON文件对话框
  int currentIndex = ui->runModeBox->currentIndex();
  if (currentIndex == 0) {
    QMessageBox::warning(
      this, QStringLiteral("提示"), QStringLiteral("模式0无参数可保存"));
    return;
  }
  //检查表格中是否有非法参数
  bool isOk = false;
  for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
    inputCheck(row, &isOk);
    if (!isOk) {
      return;
    }
  }
  QDateTime dateTime(QDateTime::currentDateTime());
  QString format = QStringLiteral("yyyyMMdd-hhmmss");
  QString timeStr = dateTime.toString(format);
  QString currentMode = tr("Mode%1_").arg(currentIndex);
  QString fileName =
    QFileDialog::getSaveFileName(this,
                                 QStringLiteral("保存参数档案"),
                                 currentMode % timeStr % QLatin1String(".json"),
                                 QStringLiteral("JSON (*.json)"));
  if (fileName.isEmpty()) {
    return;
  }
  QFile file(fileName);
  // 1.QFile::WriteOnly如果文件不存在，则创建；
  // 2.QFile::Truncate打开的同时清空
  if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("打开/保存文件失败！"));
    return;
  }
  // JSON
  QJsonObject fileObject;
  // 写入文件信息
  QJsonObject fileInfoObject;
  fileInfoObject[QStringLiteral("Time")] = timeStr;
  fileInfoObject[QStringLiteral("ModeName")] = ui->runModeBox->currentText();
  fileInfoObject[QStringLiteral("ModeID")] = currentIndex;
  fileObject[QStringLiteral("FileInfo")] = fileInfoObject;
  // 写入参数
  QJsonObject paramObject;
  for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
    // 写入一行参数
    QString name = ui->tableWidget->item(row, NAME_COL)->text();
    double val = ui->tableWidget->item(row, VAL_COL)->text().toDouble();
    paramObject.insert(name, val);
  }
  fileObject[QStringLiteral("Parameters")] = paramObject;
  //保存Json
  QJsonDocument saveDoc(fileObject);
  //保存文件
  //如果写入数据大小不等于则写入成功，大概？
  if (file.write(saveDoc.toJson()) != 0) { // 保存文件成功
    QMessageBox::information(
      this, QStringLiteral("提示"), QStringLiteral("保存文件成功"));
  } else { // 保存文件失败
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("保存文件失败！"));
  }
  file.close();
}

void
CONConfigWidget::refreshModeTable(DriverDataType::RunMode mode)
{
  //根据上一次表格Mode保存一下参数
  bool isOk = false;
  for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
    double val = inputCheck(row, &isOk);
    if (!isOk) {
      return;
    }
    //将数据保存在KEY为m_lastTableMode的map下
    // VALUE里的vector储存表格信息，其顺序同table行顺序一致
    //故可以根据行数row访问vector来保存参数
    m_tableConfig[m_lastTableMode][row].value = val;
  }
  // 清空表格中的内容（不包含表头）
  ui->tableWidget->clearContents();
  if (m_tableConfig[mode].isEmpty()) {
    // mode0 no params, just clear table
    ui->tableWidget->setRowCount(0);
    return;
  }
  //表格行数等于配置文件数组的长度
  ui->tableWidget->setRowCount(m_tableConfig[mode].size());
  //行计数器
  int rowCnt = 0;
  // 填充表格,把保存的参数刷新出来
  Q_FOREACH (const auto& row, m_tableConfig[mode]) {
    createItemsARow(rowCnt,
                    row.num,
                    row.name,
                    row.value,
                    row.isHex,
                    row.unit,
                    row.range,
                    row.defaultValue,
                    row.detail);
    rowCnt++;
  }
  //刷新上一次表格Mode记录
  //新出现的表格作为lastTable
  m_lastTableMode = mode;
}

void
CONConfigWidget::resetTableValue()
{
  int rows = ui->tableWidget->rowCount();
  for (int i = 0; i < rows; i++) {
    setTableData(i, VAL_COL, 0, false);
  }
}

void
CONConfigWidget::on_runModeBox_currentIndexChanged(int index)
{
  auto mode = static_cast<DriverDataType::RunMode>(index);
  refreshModeTable(mode);
}

void
CONConfigWidget::on_cleartTableButton_clicked()
{
  resetTableValue();
}

void
CONConfigWidget::checkDSPConfig(const QByteArray& data)
{
  DriverDataType::CONDataType configData;
  configData.byteArrayToStruct(data);
  //检查运行模式
  int readRunMode = configData.data().runMode;
  if (readRunMode != ui->runModeBox->currentIndex()) {
    QMessageBox::warning(
      this,
      QStringLiteral("警告"),
      tr("写入失败，[ERROR]Run mode set: %1, but received: %2")
        .arg(ui->runModeBox->currentIndex())
        .arg(readRunMode));
    return;
  }
  //遍历当前参数表格，
  //根据参数编号从回传的数据寻找并检查
  // read data from POD struct
  for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
    //参数编号
    int numOfParam =
      ui->tableWidget->item(row, NUM_COL)->text().toInt(nullptr, 16);
    //结构体下标0是运行模式
    //所以参数编号+1对应结构体下标
    double val = configData.at(numOfParam + 1);
    // downscale int data, convert int to double
    double scaledVal = val / DriverDataType::_IQ15;
    // compare read data with table data
    double tableVal = ui->tableWidget->item(row, VAL_COL)->text().toDouble();
    // if gap > tolerant error, we consider that write request fail
    if (abs(scaledVal - tableVal) > tolerantConfigError) {
      QMessageBox::warning(
        this,
        QStringLiteral("警告"),
        tr("写入失败，[ERROR] 编号:%1(set: %2, but received: %3)")
          .arg(numOfParam)
          .arg(tableVal)
          .arg(scaledVal));
      return;
    }
  }
  // successfully config controller & pass check
  QMessageBox::information(
    this, QStringLiteral("提示"), QStringLiteral("DSP参数写入成功"));
  //记录一下dsp的runmode
  m_xcomm->logRunMode(static_cast<DriverDataType::RunMode>(readRunMode));
}

auto
CONConfigWidget::inputCheck(const int row, bool* isOk) -> double
{
  if (ui->tableWidget->item(row, VAL_COL)->text().isEmpty()) {
    // null input
    QMessageBox::warning(
      this, QStringLiteral("错误"), tr("输入为空(第%1行)").arg(row + 1));
    ui->tableWidget->item(row, VAL_COL)->setText(QStringLiteral("0"));
    ui->tableWidget->selectRow(row);
    *isOk = false;
    return 0;
  }
  bool isDouble = false;
  double value =
    ui->tableWidget->item(row, VAL_COL)->text().toDouble(&isDouble);
  if (!isDouble) {
    // invalid input
    QMessageBox::warning(
      this, QStringLiteral("错误"), tr("非法的值(第%1行)").arg(row + 1));
    ui->tableWidget->item(row, VAL_COL)->setText(QStringLiteral("0"));
    ui->tableWidget->selectRow(row);
    *isOk = false;
    return 0;
  }
  double max = 128.0;
  double min = 0.0;
  QString range = ui->tableWidget->item(row, RANGE_COL)->text();
  // set value range
  // TBC temp solution
  if (range == QLatin1String("0-128")) {
    max = 128.0 - 1.0;
    min = 0.0;
  } else if (range == QLatin1String("0-16384")) {
    max = (2 << 13) - 1.0;
    min = 0.0;
  }
  if (value < min || value > max) {
    //如果不在设定的范围之内
    QMessageBox::warning(
      this, QStringLiteral("错误"), tr("数值超出范围(第%1行)").arg(row + 1));
    ui->tableWidget->item(row, VAL_COL)->setText(QStringLiteral("0"));
    ui->tableWidget->selectRow(row);
    *isOk = false;
    return 0;
  }
  *isOk = true;
  return value;
}

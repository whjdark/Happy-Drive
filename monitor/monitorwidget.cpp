/*
 * @Author: whj
 * @Date: 2021-02-04 22:07:13
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:04:30
 * @Description: file content
 */

#include "monitorwidget.h"

#include "ui_monitorwidget.h"

MonitorWidget::MonitorWidget(QWidget* parent, XComm* xcomm)
  : QWidget(parent)
  , ui(new Ui::MonitorWidget)
  , m_xcomm(xcomm)
  , m_updateTimer(new QTimer(this))
{
  ui->setupUi(this);

  initPanel();
  initTableHeader();
  initTableData();
  initConnetions();
}

MonitorWidget::~MonitorWidget()
{
  stopMonitor();
  delete ui;
}

void
MonitorWidget::initPanel()
{
  ui->startButton->setEnabled(true);
  ui->stopButton->setEnabled(false);
}

void
MonitorWidget::initTableHeader()
{
  //设置标题
  //表头标题用QStringList来表示
  ui->tableWidget->setWindowTitle(QStringLiteral("监视器"));
  const static QStringList headerText{ "编号",   "代号", "名称",
                                       "当前值", "单位", "详细" };
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
}

void
MonitorWidget::initTableData()
{
  // set data row count!!!
  ui->tableWidget->setRowCount(dataTableRow);
  //奇数偶数行颜色交替
  // ui->tableWidget->setAlternatingRowColors(true);
  //最后一行拉伸填充
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  //选中时一行整体选中
  ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  //初始化表格数据
  createItemsARow(FAULTCOME_ROW,
                  QStringLiteral("0x0"),
                  QStringLiteral("fault come"),
                  QStringLiteral("故障代码"),
                  m_monitorData.data().faultCome,
                  true,
                  QStringLiteral("-"),
                  QStringLiteral("无错误"));
}

void
MonitorWidget::initConnetions()
{
  using MW = MonitorWidget;
  connect(m_updateTimer, &QTimer::timeout, this, &MW::slotUpdateTable);
  connect(m_xcomm, &XComm::monitorCmd, this, &MW::slotProccessCmd);
}

void
MonitorWidget::startMonitor()
{
  if (m_xcomm->getConnectStatus() == XComm::COMM_CONNECT) {
    if (!m_updateTimer->isActive()) { //如果timer没在工作
      ui->stopButton->setEnabled(true);
      ui->startButton->setEnabled(false);
      m_updateTimer->start(refreshTime);
    }
  } else {
    QMessageBox::warning(
      this, QStringLiteral("未连接驱动"), QStringLiteral("请连接驱动后再启动"));
  }
}

void
MonitorWidget::stopMonitor()
{
  m_updateTimer->stop();
  ui->startButton->setEnabled(true);
  ui->stopButton->setEnabled(false);
}

void
MonitorWidget::slotUpdateTable()
{
  m_xcomm->command(XComm::MONITOR_REQ_DATA, QByteArray());
}

void
MonitorWidget::slotProccessCmd(const quint16 cmd, const QByteArray& data)
{
  switch (cmd) {
    case XComm::MONITOR_REQ_DATA:
      m_monitorData.byteArrayToStruct(data);
      updateTable();
      break;
    default:
      break;
  }
}

template<typename T>
void
MonitorWidget::createItemsARow(const int row,
                               const QString& num,
                               const QString& code,
                               const QString& name,
                               const T value,
                               const bool isHex,
                               const QString& unit,
                               const QString& detail)
{
  //编号
  auto* numItem = new QTableWidgetItem(num);
  ui->tableWidget->setItem(row, NUM_COL, numItem);
  //代号
  auto* codeItem = new QTableWidgetItem(code);
  ui->tableWidget->setItem(row, CODE_COL, codeItem);
  //名称
  auto* nameItem = new QTableWidgetItem(name);
  ui->tableWidget->setItem(row, NAME_COL, nameItem);
  //值
  auto* vaueItem = new QTableWidgetItem();
  if (isHex) {
    vaueItem->setText(tr("0x%1").arg(qulonglong(value), 0, 16));
  } else { // !isHex
    vaueItem->setText(tr("%1").arg(value, 0));
  }
  ui->tableWidget->setItem(row, VAL_COL, vaueItem);
  //单位
  if (!unit.isEmpty()) {
    auto* unitItem = new QTableWidgetItem(unit);
    ui->tableWidget->setItem(row, UNIT_COL, unitItem);
  }
  //详细
  if (!detail.isEmpty()) {
    auto* detailItem = new QTableWidgetItem(detail);
    ui->tableWidget->setItem(row, DETAIL_COL, detailItem);
  }
}

template<typename T>
void
MonitorWidget::setTableData(const TableRows row,
                            const TableCols col,
                            const T data,
                            const bool isHex)
{
  QTableWidgetItem* item = ui->tableWidget->item(row, col);
  if (isHex) {
    item->setText(tr("0x%1").arg(data, 0, 16));
  } else { // !isHex
    item->setText(tr("%1").arg(data, 0));
  }
}

void
MonitorWidget::updateTable()
{
  //看不见不更新
  if (!this->isVisible()) {
    return;
  }
  setTableData(FAULTCOME_ROW, VAL_COL, m_monitorData.data().faultCome, true);
}

void
MonitorWidget::on_startButton_clicked()
{
  startMonitor();
}

void
MonitorWidget::on_stopButton_clicked()
{
  stopMonitor();
}

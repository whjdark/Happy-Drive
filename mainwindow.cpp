/*
 * @Author: whj
 * @Date: 2021-01-31 20:14:17
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:04:16
 * @Description: file content
 */

#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_xcomm(new XComm(this))
  , m_console(new ConsoleMainWindow(this, m_xcomm))
  , m_monitor(new MonitorWidget(this, m_xcomm))
  , m_tracer(new TracerWidget(this, m_xcomm))
  , m_FRT(new FRT(this, m_xcomm))
  , m_checkConnectTimer(new QTimer(this))
  , m_updateStatusBarTimer(new QTimer(this))
{
  ui->setupUi(this);

  qDebug() << "mainwindow threadID : " << QThread::currentThreadId();
  qDebug() << "ideal threadcount : " << QThread::idealThreadCount();

  // dock tab化
  initTabifyDocks();
  //初初始化菜单栏
  initMenuBar();
  //初始化工具栏
  initToolBar();
  //初始化状态栏
  initStatusBar();
  //设置信号连接
  initConnections();

  //刷新状态栏 ~1s刷新一次
  m_updateStatusBarTimer->start(1000);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void
MainWindow::initTabifyDocks()
{
  //删除中央窗体
  QWidget* pCentralWidget = takeCentralWidget();
  if (pCentralWidget != nullptr) {
    delete pCentralWidget;
  }
  // dock tab set on the north(top)
  setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
  //允许嵌套dock
  setDockNestingEnabled(true);
  //监视器
  m_monitorDockWidget = new QDockWidget(QStringLiteral("监视器"), this);
  m_monitorDockWidget->setObjectName(QStringLiteral("monitorDockWidget"));
  m_monitorDockWidget->setWidget(m_monitor);
  //这样把dock wideget的标题去掉
  QWidget* monitorTitleBar = new QWidget(this);
  m_monitorDockWidget->setTitleBarWidget(monitorTitleBar);
  //储存一下DockWidget
  DockWidgets.push_back(m_monitorDockWidget);
  //数据追踪（示波器)
  m_tracerDockWidget = new QDockWidget(QStringLiteral("数据追踪"), this);
  m_tracerDockWidget->setObjectName(QStringLiteral("tracerDockWidget"));
  m_tracerDockWidget->setWidget(m_tracer);
  //这样把dock wideget的标题去掉
  QWidget* tracerTitleBar = new QWidget(this);
  m_tracerDockWidget->setTitleBarWidget(tracerTitleBar);
  //储存一下DockWidget
  DockWidgets.push_back(m_tracerDockWidget);
  // 添加至dock
  foreach (auto& widget, DockWidgets) {
    this->addDockWidget(Qt::TopDockWidgetArea, widget);
  }
  // dock tab化
  tabifyDockWidget(m_monitorDockWidget, m_tracerDockWidget);
}

void
MainWindow::initMenuBar()
{
  //菜单栏
  m_menuBar = this->menuBar();
  //通讯菜单
  m_commMenu = m_menuBar->addMenu(QStringLiteral("通讯"));
  m_conAction = m_commMenu->addAction(QStringLiteral("连接"));
  m_conAction->setIcon(QIcon(QStringLiteral(":/icon/res/icon/connect.png")));
  m_disconAction = m_commMenu->addAction(QStringLiteral("断开"));
  m_disconAction->setIcon(
    QIcon(QStringLiteral(":/icon/res/icon/disconnect.png")));
  m_commMenu->addSeparator();
  m_consoleAction = m_commMenu->addAction(QStringLiteral("控制台"));
  m_consoleAction->setIcon(
    QIcon(QStringLiteral(":/icon/res/icon/console.png")));
  //视图菜单
  m_viewMenu = m_menuBar->addMenu(QStringLiteral("视图"));
  m_defaultAction = m_viewMenu->addAction(QStringLiteral("默认布局"));
  m_closeAllAction = m_viewMenu->addAction(QStringLiteral("全部关闭"));
  m_showAllAction = m_viewMenu->addAction(QStringLiteral("全部显示"));
  //控制菜单
  m_controlMenu = m_menuBar->addMenu(QStringLiteral("控制"));
  m_startAction = m_controlMenu->addAction(QStringLiteral("运行"));
  m_startAction->setIcon(QIcon(QStringLiteral(":/icon/res/icon/start.png")));
  m_resetAction = m_controlMenu->addAction(QStringLiteral("软复位"));
  m_resetAction->setIcon(QIcon(QStringLiteral(":/icon/res/icon/reset.png")));
  m_stopAction = m_controlMenu->addAction(QStringLiteral("停止"));
  m_stopAction->setIcon(QIcon(QStringLiteral(":/icon/res/icon/stop.png")));
  //工具箱toolbox菜单
  m_toolboxMenu = m_menuBar->addMenu(QStringLiteral("工具箱"));
  m_sweepingAction = m_toolboxMenu->addAction(QStringLiteral("频率响应测试"));
  m_sweepingAction->setIcon(
    QIcon(QStringLiteral(":/icon/res/icon/sweeping.png")));
  //关于菜单
  m_helpMenu = m_menuBar->addMenu(QStringLiteral("帮助"));
  m_aboutAction = m_helpMenu->addAction(QStringLiteral("关于"));
  m_aboutAction->setIcon(QIcon(QStringLiteral(":/icon/res/icon/about.png")));
  m_aboutQtAction = m_helpMenu->addAction(QStringLiteral("关于Qt"));
  QIcon QtAboutIcon = style()->standardIcon(QStyle::SP_TitleBarMenuButton);
  m_aboutQtAction->setIcon(QtAboutIcon);
  m_manualAction = m_helpMenu->addAction(QStringLiteral("使用手册"));
}

void
MainWindow::initToolBar()
{
  //工具栏
  m_toolBar = this->addToolBar(QStringLiteral("工具栏"));
  m_toolBar->setIconSize(QSize(28, 28));
  m_toolBar->setMovable(false);
  //通讯工具栏
  m_toolBar->addAction(m_conAction);
  m_toolBar->addAction(m_disconAction);
  m_toolBar->addSeparator();
  //控制工具栏
  m_toolBar->addAction(m_startAction);
  m_toolBar->addAction(m_resetAction);
  m_toolBar->addAction(m_stopAction);
  m_toolBar->addSeparator();
  //控制台工具栏
  m_toolBar->addAction(m_consoleAction);
  m_toolBar->addSeparator();
  // toolbox工具栏
  m_toolBar->addAction(m_sweepingAction);
}

void
MainWindow::initStatusBar()
{
  //状态栏
  m_statusBar = this->statusBar();
  //不显示其内控件的边框
  m_statusBar->setStyleSheet(QStringLiteral("QStatusBar::item{border: 0px}"));
  //通讯状态
  m_commStatusLbl = new QLabel(QStringLiteral("通讯: 关闭"), this);
  m_commStatusLbl->setStyleSheet(QStringLiteral("color:red;"));
  //电机状态
  m_motorStatusLbl = new QLabel(QStringLiteral("电机: 已停止"), this);
  m_motorStatusLbl->setStyleSheet(QStringLiteral("color:red;"));
  //时钟
  currentTimeLabel = new QLabel(this);
  currentTimeLabel->setStyleSheet(QStringLiteral("color:LightSeaGreen;"));
  // add to widge
  // permanentwidget在右边
  // widget在左边
  m_statusBar->addWidget(m_commStatusLbl);
  m_statusBar->addWidget(m_motorStatusLbl);
  m_statusBar->addPermanentWidget(currentTimeLabel);
}

void
MainWindow::initConnections()
{
  using MW = MainWindow; //太长了，省略一下
  // connect menu action
  connect(m_conAction, &QAction::triggered, this, &MW::slotOpenConnectDialog);
  connect(m_aboutAction, &QAction::triggered, this, &MW::slotOpenAbout);
  connect(m_aboutQtAction, &QAction::triggered, this, &MW::slotOpenAboutQt);
  connect(m_manualAction, &QAction::triggered, this, &MW::slotOpenManuel);
  connect(m_disconAction, &QAction::triggered, this, &MW::slotDisconnect);
  connect(m_consoleAction, &QAction::triggered, m_console, &QWidget::show);
  connect(m_defaultAction, &QAction::triggered, this, &MW::slotDefaultLayout);
  connect(m_closeAllAction, &QAction::triggered, this, &MW::slotCloseAllDocks);
  connect(m_showAllAction, &QAction::triggered, this, &MW::slotShowAllDocks);
  connect(m_startAction, &QAction::triggered, this, &MW::slotStartMotor);
  connect(m_sweepingAction, &QAction::triggered, m_FRT, &QWidget::show);
  connect(m_resetAction, &QAction::triggered, this, [=]() {}); // TBC
  connect(m_stopAction, &QAction::triggered, this, &MW::slotStopMotor);
  // connect driver status info update
  connect(m_xcomm, &XComm::connectSuccess, this, &MW::slotConnectSuccess);
  connect(m_xcomm, &XComm::motorStart, this, &MW::slotMotorStartInfo);
  connect(m_xcomm, &XComm::motorStop, this, &MW::slotMotorStopInfo);
  // connect timer
  connect(m_checkConnectTimer, &QTimer::timeout, this, &MW::slotCheckConnect);
  connect(
    m_updateStatusBarTimer, &QTimer::timeout, this, &MW::slotUpdateStatusLbl);
}

void
MainWindow::slotUpdateStatusLbl()
{
  // set communicate status label
  if (m_xcomm->getConnectStatus() == XComm::COMM_IDLE) {
    m_commStatusLbl->setText(QStringLiteral("通讯: 关闭"));
    m_commStatusLbl->setStyleSheet(QStringLiteral("color:red;"));
    //连接/短线按钮使能/失效
    m_conAction->setEnabled(true);
    m_disconAction->setEnabled(false);
  } else { // m_xcomm->connectStatus() == XComm::COMM_CONNECT
    m_commStatusLbl->setText(
      tr("通讯: %1").arg(m_xcomm->getCurrentSerialPort()));
    m_commStatusLbl->setStyleSheet(QStringLiteral("color:green;"));
    //连接/短线按钮使能/失效
    m_conAction->setEnabled(false);
    m_disconAction->setEnabled(true);
  }
  // set motor status label
  if (m_xcomm->getMotorStatus() == XComm::MOTOR_RUN) {
    m_motorStatusLbl->setText(QStringLiteral("电机: 正在运行"));
    m_motorStatusLbl->setStyleSheet(QStringLiteral("color:green;"));
  } else { // m_xcomm->currMotorState() == XComm::MOTOR_STOP
    m_motorStatusLbl->setText(QStringLiteral("电机: 已停止"));
    m_motorStatusLbl->setStyleSheet(QStringLiteral("color:red;"));
  }
  //刷新时间
  QDateTime current_time = QDateTime::currentDateTime();
  //设置显示的格式
  QString format = QStringLiteral("yyyy年MM月dd日 hh:mm:ss");
  QString timestr = current_time.toString(format);
  currentTimeLabel->setText(timestr); //设置label的文本内容为时间
}

void
MainWindow::closeEvent(QCloseEvent* event)
{
  QString info;
  //检查电机运行状态
  if (m_xcomm->getMotorStatus() == XComm::MOTOR_RUN) {
    info = QStringLiteral("电机正在运行，是否退出程序?");
  } else {
    info = QStringLiteral("是否退出程序?");
  }
  //退出提示对话框
  int ret = QMessageBox::question(
    this, QStringLiteral("退出程序"), info, QMessageBox::Yes | QMessageBox::No);
  if (ret == QMessageBox::No) {
    event->ignore(); //忽略退出信号，程序继续运行
  } else if (ret == QMessageBox::Yes) {
    event->accept(); //接受退出信号，程序退出
  }
}

void
MainWindow::slotOpenAbout()
{
  AboutDialog aboutMe;
  aboutMe.exec();
}

void
MainWindow::slotOpenAboutQt()
{
  QMessageBox::aboutQt(this, QStringLiteral("关于Qt"));
}

void
MainWindow::slotOpenManuel()
{
  QMessageBox::about(
    this, QStringLiteral("使用手册"), QStringLiteral("待开发"));
}

void
MainWindow::slotConnectSuccess()
{
  QMessageBox::warning(
    this, QStringLiteral("提示"), QStringLiteral("连接成功"));
  //连接成功后默认启动监视器
  m_monitor->startMonitor();
}

void
MainWindow::slotOpenConnectDialog()
{
  //连接设置
  ConnectDialog connectDialog(this);
  connectDialog.exec(); //阻止对其他窗口的操作
  if (connectDialog.isConnect()) {
    //配置串口并进行连接
    m_xcomm->configSerialPort(connectDialog.getConfig());
    m_xcomm->connectDriver();
    //连接状态检查
    m_checkConnectTimer->start(3100);
  }
}

void
MainWindow::slotDisconnect()
{
  if (m_xcomm->getMotorStatus() == XComm::MOTOR_RUN) {
    QMessageBox msgBox(this);
    msgBox.setFixedSize(200, 200);
    msgBox.setWindowTitle(QStringLiteral("警告"));
    msgBox.setText(QStringLiteral("请确定电机运行状态"));
    msgBox.setInformativeText(QStringLiteral("电机正在运行，是否断开连接?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Cancel) {
      return;
    }
  }
  m_monitor->stopMonitor();
  m_xcomm->disconnectDriver();
  //不需要连接状态检查
  m_checkConnectTimer->stop();
}

void
MainWindow::slotDefaultLayout()
{
  slotCloseAllDocks();
  // 停止悬浮，返回mainwindow
  foreach (auto& widget, DockWidgets) {
    widget->setFloating(false);
  }
  // dock tab化
  tabifyDockWidget(m_tracerDockWidget, m_monitorDockWidget);
  slotShowAllDocks();
}

void
MainWindow::slotCloseAllDocks()
{
  foreach (auto& dock, DockWidgets) {
    dock->close();
  }
}

void
MainWindow::slotShowAllDocks()
{
  foreach (auto& dock, DockWidgets) {
    dock->show();
  }
}

void
MainWindow::slotCheckConnect()
{
  if (m_xcomm->getConnectStatus() == XComm::COMM_IDLE) {
    QMessageBox::warning(
      this, QStringLiteral("错误"), QStringLiteral("连接失败"));
    m_xcomm->disconnectDriver();
  }
  //不需要连接状态检查
  m_checkConnectTimer->stop();
}

void
MainWindow::slotStartMotor()
{
  //直接调用tracer的启动按钮的槽函数
  m_tracer->setStartButtonOn();
}

void
MainWindow::slotStopMotor()
{
  //直接调用tracer的停止按钮的槽函数
  m_tracer->setStopButtonOn();
}

void
MainWindow::slotMotorStartInfo()
{
  QMessageBox::information(
    this, QStringLiteral("提示"), QStringLiteral("电机已启动"));
}

void
MainWindow::slotMotorStopInfo()
{
  QMessageBox::information(
    this, QStringLiteral("提示"), QStringLiteral("电机已停止"));
}

﻿/*
 * @Author: whj
 * @Date: 2021-01-31 20:14:17
 * @LastEditors: whj
 * @LastEditTime: 2021-03-08 13:04:24
 * @Description: file content
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QDockWidget>
#include <QLabel>
#include <QMainWindow>
#include <QStyle>
#include <QStyleFactory>
#include <QTimer>
#include <QToolBar>
#include <QVector>

#include "aboutdialog/aboutdialog.h"
#include "connectDialog/connectdialog.h"
#include "console/consolemainwindow.h"
#include "datatracer/datatracer.h"
#include "monitor/monitorwidget.h"
#include "xcomm/xcomm.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private Q_SLOTS:
  void slotOpenConnectDialog();
  void slotOpenAbout();
  void slotOpenAboutQt();
  void slotOpenManuel();
  void slotConnectSuccess();
  void slotMotorStartInfo();
  void slotMotorStopInfo();
  void slotDisconnect();
  void slotDefaultLayout();
  void slotCloseAllDocks();
  void slotShowAllDocks();
  void slotCheckConnect();
  void slotStartMotor();
  void slotStopMotor();
  void slotUpdateStatusLabel();

protected:
  void closeEvent(QCloseEvent* event) override;

private:
  void initTabifyDocks();
  void initMenuBar();
  void initToolBar();
  void initStatusBar();
  void initConnections();

private:
  Ui::MainWindow* ui;
  XComm* m_xcomm;

  ConsoleMainWindow* m_console;
  MonitorWidget* m_monitor;
  TracerWidget* m_tracer;

  QVector<QDockWidget*> DockWidgets;
  QDockWidget* m_monitorDockWidget;
  QDockWidget* m_tracerDockWidget;

  QToolBar* m_toolBar;

  QStatusBar* m_statusBar;
  QLabel* m_commStatusLbl;
  QLabel* m_motorStatusLbl;
  QLabel* currentTimeLabel;

  QMenuBar* m_menuBar;
  QMenu* m_commMenu;
  QAction* m_conAction;
  QAction* m_disconAction;
  QAction* m_consoleAction;

  QMenu* m_viewMenu;
  QAction* m_defaultAction;
  QAction* m_closeAllAction;
  QAction* m_showAllAction;

  QMenu* m_controlMenu;
  QAction* m_startAction;
  QAction* m_resetAction;
  QAction* m_stopAction;

  QMenu* m_helpMenu;
  QAction* m_aboutAction;
  QAction* m_aboutQtAction;
  QAction* m_manualAction;

  QTimer* m_checkConnectTimer;
  QTimer* m_updateStatusBarTimer;
};
#endif // MAINWINDOW_H

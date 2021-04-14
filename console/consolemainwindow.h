#ifndef CONSOLEMAINWINDOW_H
#define CONSOLEMAINWINDOW_H

#include <QMainWindow>

#include "consolewidget.h"

namespace Ui {
class ConsoleMainWindow;
}

class ConsoleMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit ConsoleMainWindow(QWidget* parent = nullptr, XComm* xcomm = nullptr);
  ~ConsoleMainWindow();

private:
  Ui::ConsoleMainWindow* ui;
  ConsoleWidget* m_consoleWidget;
};

#endif // CONSOLEMAINWINDOW_H

#include "consolemainwindow.h"

#include "ui_consolemainwindow.h"

ConsoleMainWindow::ConsoleMainWindow(QWidget* parent, XComm* xcomm)
  : QMainWindow(parent)
  , ui(new Ui::ConsoleMainWindow)
  , m_consoleWidget(new ConsoleWidget(this, xcomm))
{
  ui->setupUi(this);

  this->setCentralWidget(m_consoleWidget);
}

ConsoleMainWindow::~ConsoleMainWindow()
{
  delete ui;
}

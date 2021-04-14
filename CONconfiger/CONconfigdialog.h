#ifndef CONCONFIGDIALOG_H
#define CONCONFIGDIALOG_H

#include <QDialog>
#include <QPushButton>

#include "CONconfigwidget.h"

namespace Ui {
class CONConfigDialog;
}

class CONConfigQDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CONConfigQDialog(QWidget* parent = nullptr, XComm* xcomm = nullptr);
  ~CONConfigQDialog();

private:
  Ui::CONConfigDialog* ui;
  CONConfigWidget* m_conConfigWidget;
};

#endif // CONCONFIGDIALOG_H

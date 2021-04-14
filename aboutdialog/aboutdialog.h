#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

#include "versionInfo.h"

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AboutDialog(QWidget* parent = nullptr);
  ~AboutDialog();

private Q_SLOTS:
  void on_pushButton_clicked();

private:
  Ui::AboutDialog* ui;
};

#endif // ABOUTDIALOG_H

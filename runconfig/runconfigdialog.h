#ifndef RUNCONFIGDIALOG_H
#define RUNCONFIGDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QPushButton>

#include "../BaseDataType/driverdatatype.h"

namespace Ui {
class RunConfigDialog;
}

class RunConfigDialog : public QDialog
{
  Q_OBJECT
public:
  explicit RunConfigDialog(QWidget* parent = nullptr);
  ~RunConfigDialog();

  void setRunModeInfo(const QString& runMode);
  const DriverDataType::RunConfigType& getRunConfig() const;

private Q_SLOTS:
  void on_freqComboBox_currentTextChanged(const QString& text);
  void on_RunPushButton_clicked();
  void on_cancelPushButton_clicked();

private:
  Q_CONSTEXPR static int defaultSampleFreq = 400;
  Q_CONSTEXPR static int defaultSamplePonits = 1024;

  Ui::RunConfigDialog* ui;
  DriverDataType::RunConfigType m_runConfig; // 400Hz
};

#endif // RUNCONFIGDIALOG_H

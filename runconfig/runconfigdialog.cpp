﻿#include "runconfigdialog.h"

#include "ui_runconfigdialog.h"

RunConfigDialog::RunConfigDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::RunConfigDialog)
  , m_runConfig(defaultSampleFreq, defaultSamplePonits)
{
  ui->setupUi(this);
  setWindowTitle(QStringLiteral("运行参数配置"));

  setFixedSize(QSize(400, 300));
  setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

RunConfigDialog::~RunConfigDialog()
{
  delete ui;
}

void
RunConfigDialog::setRunModeInfo(const QString& runMode)
{
  ui->runModeText->setText(
    tr("<font color=\"#FF0000\" size=4>%1</font>").arg(runMode));
}

auto
RunConfigDialog::getRunConfig() const -> const DriverDataType::RunConfigType&
{
  return m_runConfig;
}

void
RunConfigDialog::on_freqComboBox_currentTextChanged(const QString& text)
{
  bool isOk = false;
  int sampleFreq = text.toInt(&isOk);
  if (isOk && sampleFreq > 0) {
    //显示采样周期
    QString sampleCycle = QString::number(1.0 / sampleFreq, 'f', 5);
    ui->cycleText->setText(sampleCycle);
  }
}

void
RunConfigDialog::on_RunPushButton_clicked()
{
  bool isOk = false;
  int sampleFreq = ui->freqComboBox->currentText().toInt(&isOk);
  if (!isOk) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("非法的值"));
    return;
  }
  if (sampleFreq <= 0) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("frequency must > 0"));
    return;
  }
  m_runConfig.data().m_sampleFreq = sampleFreq;
  accept();
}

void
RunConfigDialog::on_cancelPushButton_clicked()
{
  reject();
}
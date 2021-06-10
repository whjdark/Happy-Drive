#include "runconfigdialog.h"

#include "ui_runconfigdialog.h"

RunConfigDialog::RunConfigDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::RunConfigDialog)
  , m_runConfig(defaultSampleFreq, defaultSamplePonits)
{
  ui->setupUi(this);
  setWindowTitle(QStringLiteral("运行参数配置"));

  setFixedSize(QSize(500, 300));
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
    tr("<font color=\"#FF0000\" size=3>%1</font>").arg(runMode));
}

void
RunConfigDialog::setSampleFrequency(int frequency)
{
  ui->freqComboBox->setCurrentText(QString::number(frequency));
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
    QString samplePeriod = QString::number(1.0 / sampleFreq, 'f', 5);
    ui->periodText->setText(samplePeriod);
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
  if (sampleFreq <= 0 || sampleFreq > maxSampleFreq) {
    QMessageBox::warning(
      this, QStringLiteral("警告"), QStringLiteral("frequency out of range"));
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

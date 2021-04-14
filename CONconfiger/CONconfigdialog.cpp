#include "CONconfigdialog.h"

#include "ui_CONconfigdialog.h"

CONConfigQDialog::CONConfigQDialog(QWidget* parent, XComm* xcomm)
  : QDialog(parent)
  , ui(new Ui::CONConfigDialog)
  , m_conConfigWidget(new CONConfigWidget(this, xcomm))
{
  ui->setupUi(this);

  // add configWidget to window
  ui->gridLayout->addWidget(m_conConfigWidget, 0, 0, 1, 3);
  setWindowTitle(QStringLiteral("控制器参数配置"));

  ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QStringLiteral("确认"));
  ui->buttonBox->button(QDialogButtonBox::Ok)
    ->setIcon(QIcon(QStringLiteral(":/icon/res/icon/confirm.png")));
  ui->buttonBox->button(QDialogButtonBox::Cancel)
    ->setText(QStringLiteral("取消"));
  ui->buttonBox->button(QDialogButtonBox::Cancel)
    ->setIcon(QIcon(QStringLiteral(":/icon/res/icon/cancel.png")));

  setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint); // 不显示问号
}

CONConfigQDialog::~CONConfigQDialog()
{
  delete ui;
}

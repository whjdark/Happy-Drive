#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::AboutDialog)
{
  ui->setupUi(this);

  setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint); // 不显示问号
  setFixedSize(500, 300); // 不能伸缩的对话框

  ui->infoLabel->setText("<b>" SOFTWARE_NAME "</b><br>"
                         "Version: " SOFTWARE_VERSION " (" BUILD_VERSION ")<br>"
                         "Email: boeingA380@126.com<br>"
                         "Source Page: <a href=\"\">View Code On <br>"
                         "Home Page: <a href=\"\">View Page On "
                         "GitHub</a><br>"
                         "Build Date: " __DATE__ "<br>" COPYRIGHT);
}

AboutDialog::~AboutDialog()
{
  delete ui;
}

void
AboutDialog::on_pushButton_clicked()
{
  this->close();
}

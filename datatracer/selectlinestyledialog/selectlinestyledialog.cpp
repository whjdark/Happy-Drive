#include "selectlinestyledialog.h"

#include "ui_selectlinestyledialog.h"

SelectLineStyleDialog::SelectLineStyleDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SelectLineStyleDialog)
{
  ui->setupUi(this);
  // set dialogbutton title & icon
  ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QStringLiteral("确认"));
  ui->buttonBox->button(QDialogButtonBox::Ok)
    ->setIcon(QIcon(QStringLiteral(":/icon/res/icon/confirm.png")));
  ui->buttonBox->button(QDialogButtonBox::Cancel)
    ->setText(QStringLiteral("取消"));
  ui->buttonBox->button(QDialogButtonBox::Cancel)
    ->setIcon(QIcon(QStringLiteral(":/icon/res/icon/cancel.png")));
  setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

SelectLineStyleDialog::~SelectLineStyleDialog()
{
  delete ui;
}

auto
SelectLineStyleDialog::getStyle() const -> QPair<int, QCPScatterStyle>
{
  int lineStyle = ui->lineStyleComboBox->currentIndex();
  int scatterShape = ui->scatterShapeComboBox->currentIndex();
  double scatterSize = ui->scatterSizeDoubleSpinBox->value();
  QCPScatterStyle scatterStyle;
  scatterStyle.setShape(shapes[scatterShape]);
  scatterStyle.setSize(scatterSize);
  auto ret = QPair<int, QCPScatterStyle>(lineStyle, scatterStyle);
  return ret;
}

void
SelectLineStyleDialog::restoreStyle(int lineStyle, QCPScatterStyle scatterStyle)
{
  ui->lineStyleComboBox->setCurrentIndex(lineStyle);
  ui->scatterShapeComboBox->setCurrentIndex(scatterStyle.shape());
  ui->scatterSizeDoubleSpinBox->setValue(scatterStyle.size());
}

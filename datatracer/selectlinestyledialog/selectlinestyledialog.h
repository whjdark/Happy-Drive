#ifndef SELECTLINESTYLEDIALOG_H
#define SELECTLINESTYLEDIALOG_H

#include <QDialog>
#include <QPair>

#include "../../QCustomPlot/qcustomplot.h"

namespace Ui {
class SelectLineStyleDialog;
}

class SelectLineStyleDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SelectLineStyleDialog(QWidget* parent = nullptr);
  ~SelectLineStyleDialog();

  QPair<int, QCPScatterStyle> getStyle() const;
  void restoreStyle(int lineStyle, QCPScatterStyle scatterShape);

private:
  // line style array
  const std::array<QCPScatterStyle::ScatterShape, 16> shapes{
    QCPScatterStyle::ssNone,
    QCPScatterStyle::ssDot,
    QCPScatterStyle::ssCross,
    QCPScatterStyle::ssPlus,
    QCPScatterStyle::ssCircle,
    QCPScatterStyle::ssDisc,
    QCPScatterStyle::ssSquare,
    QCPScatterStyle::ssDiamond,
    QCPScatterStyle::ssStar,
    QCPScatterStyle::ssTriangle,
    QCPScatterStyle::ssTriangleInverted,
    QCPScatterStyle::ssCrossSquare,
    QCPScatterStyle::ssPlusSquare,
    QCPScatterStyle::ssCrossCircle,
    QCPScatterStyle::ssPlusCircle,
    QCPScatterStyle::ssPeace
  };

  Ui::SelectLineStyleDialog* ui;
};

#endif // SELECTLINESTYLEDIALOG_H

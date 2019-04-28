#ifndef UPDATEPIPEWIDTHDIALOG_H
#define UPDATEPIPEWIDTHDIALOG_H

#include <QtWidgets>

namespace Ui {
class UpdatePipeWidthDialog;
}

class UpdatePipeWidthDialog : public QDialog {
  Q_OBJECT

public:
  explicit UpdatePipeWidthDialog(QWidget *parent = nullptr);
  ~UpdatePipeWidthDialog();

  Ui::UpdatePipeWidthDialog *ui;
};

#endif // UPDATEPIPEWIDTHDIALOG_H

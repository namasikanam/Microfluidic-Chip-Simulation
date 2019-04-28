#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtWidgets>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = nullptr);
  ~Dialog();

  void checkSpinBoxes();

private slots:
  void on_chipSize_valueChanged(int arg1);
  void on_Dialog_accepted();

private:
  Ui::Dialog *ui;
};

#endif // DIALOG_H

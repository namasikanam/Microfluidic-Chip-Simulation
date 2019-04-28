#include "dialog.h"
#include "chip.h"
#include "mainwindow.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);

  ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  ui->chipSize->setFocus();

  for (auto widget :
       findChildren<QSpinBox *>(QRegularExpression("input.|output.")))
    connect(widget, qOverload<int>(&QSpinBox::valueChanged),
            [this](int) { checkSpinBoxes(); });
}

Dialog::~Dialog() { delete ui; }

void Dialog::checkSpinBoxes() {
  ui->buttonBox->button(QDialogButtonBox::Ok)
      ->setEnabled(ui->input1->value() != ui->input2->value() &&
                   ui->output1->value() != ui->output2->value() &&
                   ui->output1->value() != ui->output3->value() &&
                   ui->output2->value() != ui->output3->value());
}

void Dialog::on_chipSize_valueChanged(int length) {
  for (auto widget :
       findChildren<QSpinBox *>(QRegularExpression("input.|output.")))
    widget->setMaximum(length - 1);
}

void Dialog::on_Dialog_accepted() {
  QList<int> inputs, outputs;
  inputs.append(ui->input1->value());
  inputs.append(ui->input2->value());
  outputs.append(ui->output1->value());
  outputs.append(ui->output2->value());
  outputs.append(ui->output3->value());
  MainWindow::getMainWindow()->initialize(ui->chipSize->value(), inputs,
                                          outputs);
}

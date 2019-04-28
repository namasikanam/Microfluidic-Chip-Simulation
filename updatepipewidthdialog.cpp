#include "updatepipewidthdialog.h"
#include "mainwindow.h"
#include "ui_updatepipewidthdialog.h"

UpdatePipeWidthDialog::UpdatePipeWidthDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::UpdatePipeWidthDialog) {
  ui->setupUi(this);
  setWindowModality(Qt::WindowModal);
}

UpdatePipeWidthDialog::~UpdatePipeWidthDialog() { delete ui; }

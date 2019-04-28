#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chip.h"
#include "dialog.h"
#include "myaction.h"
#include "widget.h"
#include <QMainWindow>
#include <QtWidgets>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  static MainWindow *getMainWindow();
  void showMessageInStatusBar(QString Message);
  void clearStatusBar();
  void initialize(int chipSize, QList<int> inputs, QList<int> outputs);
  void clearProcessed();
  void updates();

  bool processed;

private slots:
  void process();
  void maintain();

private:
  Ui::MainWindow *ui;

  Dialog *initDialog;
  QTabWidget *qTabWidget;
  Widget *widgetVelocity, *widgetDischarge, *widgetConcentration;
  MyAction *actionInitialize, *actionProcess, *actionMaintain;
  Chip *chip;
};

#endif // MAINWINDOW_H

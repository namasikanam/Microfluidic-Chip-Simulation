#include "mainwindow.h"
#include "chip.h"
#include "myaction.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include <QtWidgets>

static MainWindow *pMainWindow;

static QMetaObject::Connection processConnect, maintainConnect;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), initDialog(new Dialog(this)),
      chip(new Chip) {
  ui->setupUi(this);

  //单例模式
  pMainWindow = this;

  // Tab Widget
  qTabWidget = new QTabWidget(this);
  setCentralWidget(qTabWidget);
  qTabWidget->setMouseTracking(true);

  widgetVelocity = new Widget(this);
  qTabWidget->addTab(widgetVelocity, "&Velocity");
  widgetDischarge = new Widget(this);
  qTabWidget->addTab(widgetDischarge, "&Discharge");
  widgetConcentration = new Widget(this);
  qTabWidget->addTab(widgetConcentration, "&Concentration");

  // Initialize
  actionInitialize = new MyAction(this);
  actionInitialize->setText("&Initialize");
  ui->mainToolBar->addAction(actionInitialize);

  // Process
  processed = false;
  actionProcess = new MyAction(this);
  actionProcess->setText("&Process");
  ui->mainToolBar->addAction(actionProcess);

  // Maintain
  actionMaintain = new MyAction(this);
  actionMaintain->setText("&Maintain");
  ui->mainToolBar->addAction(actionMaintain);

  // connect
  connect(actionInitialize, &MyAction::triggered,
          [this] { initDialog->show(); });
}

MainWindow::~MainWindow() {
  delete ui;
  delete chip;
}

MainWindow *MainWindow::getMainWindow() { return pMainWindow; }

void MainWindow::showMessageInStatusBar(QString Message) {
  ui->statusBar->showMessage(Message);
}

void MainWindow::initialize(int chipSize, QList<int> inputs,
                            QList<int> outputs) {
  disconnect(processConnect);
  processConnect =
      connect(actionProcess, &MyAction::triggered, this, &MainWindow::process);
  disconnect(maintainConnect);

  chip->initialize(chipSize, inputs, outputs);
  clearProcessed();

  widgetVelocity->initialize();
  widgetDischarge->initialize();
  widgetConcentration->initialize();
}

void MainWindow::clearProcessed() {
  processed = false;
  disconnect(maintainConnect);
}

void MainWindow::clearStatusBar() { statusBar()->clearMessage(); }

void MainWindow::updates() {
  bool processEnabled = true;
  int pipeCount;
  pipeCount = 0;
  for (int j = 0; j < chip->chipSize; ++j)
    pipeCount += chip->downPipeWidth[0][j] > 0;
  processEnabled &= pipeCount == 2;
  pipeCount = 0;
  for (int j = 0; j < chip->chipSize; ++j)
    pipeCount += chip->downPipeWidth[chip->chipSize][j] > 0;
  processEnabled &= pipeCount == 3;

  if (processEnabled)
    processEnabled &= chip->connected();

  disconnect(processConnect);
  if (processEnabled)
    processConnect = connect(actionProcess, &MyAction::triggered, this,
                             &MainWindow::process);

  clearProcessed();
  widgetVelocity->update();
  widgetDischarge->update();
  widgetConcentration->update();
};

void MainWindow::process() {
  chip->process();
  processed = true;
  disconnect(maintainConnect);
  maintainConnect = connect(actionMaintain, &MyAction::triggered, this,
                            &MainWindow::maintain);

  memcpy(widgetVelocity->downAnswer, chip->downVelocity,
         sizeof(chip->downVelocity));
  memcpy(widgetVelocity->rightAnswer, chip->rightVelocity,
         sizeof(chip->rightVelocity));
  memcpy(widgetDischarge->downAnswer, chip->downDischarge,
         sizeof(chip->downDischarge));
  memcpy(widgetDischarge->rightAnswer, chip->rightDischarge,
         sizeof(chip->rightDischarge));
  memcpy(widgetConcentration->downAnswer, chip->downConcentration,
         sizeof(chip->downConcentration));
  memcpy(widgetConcentration->rightAnswer, chip->rightConcentration,
         sizeof(chip->rightConcentration));

  widgetVelocity->process();
  widgetDischarge->process();
  widgetConcentration->process();
}

void MainWindow::maintain() {
  //  qDebug("Hi I'm here");

  QList<qreal> ansVelocity, ansDischarge, ansConcentration;
  for (int i = 0; i < 3; ++i) {
    ansVelocity.append(widgetVelocity->qDoubleSpinBoxes[i]->value());
    ansDischarge.append(widgetDischarge->qDoubleSpinBoxes[i]->value());
    ansConcentration.append(widgetConcentration->qDoubleSpinBoxes[i]->value());
  }
  chip->maintain(ansVelocity, ansDischarge, ansConcentration);

  memcpy(widgetVelocity->downAnswer, chip->downVelocity,
         sizeof(chip->downVelocity));
  memcpy(widgetVelocity->rightAnswer, chip->rightVelocity,
         sizeof(chip->rightVelocity));
  memcpy(widgetDischarge->downAnswer, chip->downDischarge,
         sizeof(chip->downDischarge));
  memcpy(widgetDischarge->rightAnswer, chip->rightDischarge,
         sizeof(chip->rightDischarge));
  memcpy(widgetConcentration->downAnswer, chip->downConcentration,
         sizeof(chip->downConcentration));
  memcpy(widgetConcentration->rightAnswer, chip->rightConcentration,
         sizeof(chip->rightConcentration));

  widgetVelocity->maintain();
  widgetDischarge->maintain();
  widgetConcentration->maintain();
}

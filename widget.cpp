#include "widget.h"
#include "chip.h"
#include "mainwindow.h"
#include "ui_updatepipewidthdialog.h"
#include "updatepipewidthdialog.h"
#include <bits/stdc++.h>
using namespace std;

const static int inf = 0x7fffffff;
const static qreal eps = 1e-12;
const static qreal marginRatio = 0.03;
static qreal pointsPerum;
static qreal length, pointSize, standardPipeWidth;
static qreal lft, top;

Widget::Widget(QWidget *parent) : QWidget(parent) {
  setMouseTracking(true);

  for (int i = 0; i < 3; ++i) {
    auto *qDoubleSpinBox = new QDoubleSpinBox(this);
    qDoubleSpinBox->setReadOnly(false);
    qDoubleSpinBox->setSingleStep(1e-3);
    qDoubleSpinBox->setMinimum(0.0);
    qDoubleSpinBox->setMaximum(1.0);
    qDoubleSpinBox->setGeometry(0, 0, 75, 30);
    qDoubleSpinBox->setDecimals(3);
    qDoubleSpinBox->hide();
    qDoubleSpinBoxes.append(qDoubleSpinBox);
  }
}

Widget::~Widget() {}

void Widget::mouseMoveEvent(QMouseEvent *e) {
  Chip *chip = Chip::getChip();
  MainWindow *mainWindow = MainWindow::getMainWindow();

  if (!chip->chipSize)
    return;

  // 计算初始化
  qreal x, y;

  y = top;
  for (int i = 0; i < chip->chipSize + 1; ++i) {
    x = lft;
    for (int j = 0; j < chip->chipSize; ++j) {
      if (QRectF(x - pointSize / 2, y + pointSize / 2, standardPipeWidth,
                 length)
              .contains(e->localPos()) &&
          chip->downPipeWidth[i][j] > 0) {
        mainWindow->showMessageInStatusBar(
            "Width = " + QString::number(chip->downPipeWidth[i][j]));
        return;
      }
      x += length + pointSize;
    }
    y += length + pointSize;
  }

  y = top + length + pointSize;
  for (int i = 1; i <= chip->chipSize; ++i) {
    x = lft;
    for (int j = 0; j < chip->chipSize - 1; ++j) {
      if (QRectF(x + pointSize / 2, y - pointSize / 2, length,
                 standardPipeWidth)
              .contains(e->localPos()) &&
          chip->rightPipeWidth[i][j] > 0) {
        mainWindow->showMessageInStatusBar(
            "Width: " + QString::number(chip->rightPipeWidth[i][j]));
        return;
      }
      x += length + pointSize;
    }
    y += length + pointSize;
  }
  mainWindow->clearStatusBar();
}

void Widget::debug(QDoubleSpinBox *p) { p->setValue(0.5); }

void Widget::paintEvent(QPaintEvent *ev) {
  Chip *chip = Chip::getChip();
  MainWindow *mainWindow = MainWindow::getMainWindow();
  if (!chip->chipSize)
    return;

  QPainter p(this);
  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                   QPainter::SmoothPixmapTransform);

  // 计算相关
  pointsPerum = min(width() * (1 - 2 * marginRatio) /
                        ((chip->chipSize + 1) * 1600 + chip->chipSize * 200),
                    (height() * (1 - 2 * marginRatio) - 30) /
                        ((chip->chipSize + 1) * 1600 + chip->chipSize * 200));
  length = 1600 * pointsPerum;
  pointSize = 200 * pointsPerum;
  standardPipeWidth = 200 * pointsPerum;
  lft = width() * marginRatio + length + pointSize / 2;
  top = height() * marginRatio - pointSize / 2;
  qreal x, y;

  // 画管子
  p.setPen(Qt::NoPen);

  int pipeWidth;
  y = top;
  for (int i = 0; i < chip->chipSize + 1; ++i) {
    x = lft;
    for (int j = 0; j < chip->chipSize; ++j) {
      if ((pipeWidth = chip->downPipeWidth[i][j]) > 0) {
        if (mainWindow->processed && chip->downID[i][j])
          p.setBrush(
              QBrush(QColor::fromHslF(abs(downAnswer[i][j]) / 2, 0.9, 0.5)));
        else
          p.setBrush(Qt::white);
        p.drawRect(QRectF(x - pipeWidth * pointsPerum / 2, y + pointSize / 2,
                          pipeWidth * pointsPerum, length));
      }
      x += length + pointSize;
    }
    y += length + pointSize;
  }

  //  for (int y = 1; y <= chip->chipSize; ++y) {
  //    QString row;
  //    for (int x = 0; x < chip->chipSize - 1; ++x)
  //      row += ' ' + QString::number(rightAnswer[y][x]);
  //    qDebug() << row;
  //  }

  y = top + length + pointSize;
  for (int i = 1; i <= chip->chipSize; ++i) {
    x = lft;
    for (int j = 0; j < chip->chipSize - 1; ++j) {
      if ((pipeWidth = chip->rightPipeWidth[i][j]) > 0) {
        if (mainWindow->processed && chip->rightID[i][j])
          p.setBrush(
              QBrush(QColor::fromHslF(abs(rightAnswer[i][j]) / 2, 0.9, 0.5)));
        else
          p.setBrush(Qt::white);
        p.drawRect(QRectF(x + pointSize / 2, y - pipeWidth * pointsPerum / 2.0,
                          length, pipeWidth * pointsPerum));
      }
      x += length + pointSize;
    }
    y += length + pointSize;
  }

  // 画交点
  p.setBrush(Qt::white);
  p.setPen(Qt::black);
  y = top + length + pointSize;
  for (int i = 0; i < chip->chipSize; ++i) {
    x = lft;
    for (int j = 0; j < chip->chipSize; ++j) {
      p.drawRect(
          QRectF(x - pointSize / 2, y - pointSize / 2, pointSize, pointSize));
      x += length + pointSize;
    }
    y += length + pointSize;
  }

  // 输出答案
  if (mainWindow->processed) {
    for (int i = 0; i < 3; ++i) {
      auto *qDoubleSpinBox = qDoubleSpinBoxes[i];
      qDoubleSpinBox->setGeometry(
          static_cast<int>(lft + chip->outputs[i] * (length + pointSize) -
                           75 / 2),
          static_cast<int>(y - pointSize / 2), 75, 30);
      qDoubleSpinBox->show();
    }
  }
}

void Widget::initialize() {
  // 约束条件
  // 常规约束
  // 200um \ge 1
  // 1600um \ge QDoubleSpinBox.width()
  // 注意管子可以膨胀到两边!
  int chipSize = Chip::getChip()->chipSize;
  qreal minPointsPerum = max(1.0 / 200, 75 / 1600.0);
  int minw = static_cast<int>(((chipSize + 1) * 1600 * minPointsPerum +
                               200 * chipSize * minPointsPerum) /
                              (1 - 2 * marginRatio)),
      minh = static_cast<int>(((chipSize + 1) * 1600 * minPointsPerum +
                               chipSize * 200 * minPointsPerum + 30) /
                              (1 - 2 * marginRatio));
  setMinimumSize(minw, minh);

  for (auto *qDoubleSpinBox : qDoubleSpinBoxes)
    qDoubleSpinBox->hide();
  update();
}

void Widget::process() {
  Chip *chip = Chip::getChip();
  for (int i = 0; i < 3; ++i)
    qDoubleSpinBoxes[i]->setValue(downAnswer[chip->chipSize][chip->outputs[i]]);
  update();
}

void Widget::maintain() {
  Chip *chip = Chip::getChip();
  for (int i = 0; i < 3; ++i)
    qDoubleSpinBoxes[i]->setValue(downAnswer[chip->chipSize][chip->outputs[i]]);
  update();
}

void Widget::mousePressEvent(QMouseEvent *e) {
  // 一旦修改, 就需要重新计算!
  Chip *chip = Chip::getChip();
  MainWindow *mainWindow = MainWindow::getMainWindow();

  if (!chip->chipSize)
    return;

  // 计算初始化
  qreal x, y;

  // 函数初始化
  auto adjacentLimit = [](int width) -> int {
    return (1600 + 100 * 2 - ((width < 0 ? 20 : width) + 1) / 2 - 200) * 2;
  };
  auto adjacentMin = [adjacentLimit](int &a, int b) {
    a = min(a, adjacentLimit(b));
  };

  if (e->button() == Qt::LeftButton) {
    y = top;
    for (int i = 0; i < chip->chipSize + 1; ++i) {
      x = lft;
      for (int j = 0; j < chip->chipSize; ++j) {
        if (QRectF(x - pointSize / 2, y + pointSize / 2, standardPipeWidth,
                   length)
                .contains(e->localPos())) {
          // 配置管道宽度

          if ((chip->downPipeWidth[i][j] = -chip->downPipeWidth[i][j]) > 0) {
            if (j > 0)
              adjacentMin(chip->downPipeWidth[i][j],
                          chip->downPipeWidth[i][j - 1]);
            if (j + 1 < chip->chipSize)
              adjacentMin(chip->downPipeWidth[i][j],
                          chip->downPipeWidth[i][j + 1]);
          }

          mainWindow->updates();
          return;
        }
        x += length + pointSize;
      }
      y += length + pointSize;
    }

    y = top + length + pointSize;
    for (int i = 1; i <= chip->chipSize; ++i) {
      x = lft;
      for (int j = 0; j < chip->chipSize - 1; ++j) {
        if (QRectF(x + pointSize / 2, y - pointSize / 2, length,
                   standardPipeWidth)
                .contains(e->localPos())) {
          if ((chip->rightPipeWidth[i][j] = -chip->rightPipeWidth[i][j]) > 0) {
            if (i - 1 >= 1)
              adjacentMin(chip->rightPipeWidth[i][j],
                          chip->rightPipeWidth[i - 1][j]);
            if (i + 1 <= chip->chipSize)
              adjacentMin(chip->rightPipeWidth[i][j],
                          chip->rightPipeWidth[i + 1][j]);
          }

          mainWindow->updates();
          return;
        }
        x += length + pointSize;
      }
      y += length + pointSize;
    }
  } else if (e->button() == Qt::RightButton) {
    UpdatePipeWidthDialog updatePipeWidthDialog(this);
    y = top;
    for (int i = 0; i < chip->chipSize + 1; ++i) {
      x = lft;
      for (int j = 0; j < chip->chipSize; ++j) {
        if (chip->downPipeWidth[i][j] > 0 &&
            QRectF(x - pointSize / 2, y + pointSize / 2, standardPipeWidth,
                   length)
                .contains(e->localPos())) {
          updatePipeWidthDialog.ui->spinBox->setValue(
              chip->downPipeWidth[i][j]);
          updatePipeWidthDialog.ui->spinBox->setMaximum(
              min(j ? adjacentLimit(chip->downPipeWidth[i][j - 1]) : inf,
                  j + 1 < chip->chipSize
                      ? adjacentLimit(chip->downPipeWidth[i][j + 1])
                      : inf));
          connect(&updatePipeWidthDialog, &UpdatePipeWidthDialog::accepted,
                  [&updatePipeWidthDialog, chip, i, j, mainWindow] {
                    chip->downPipeWidth[i][j] =
                        updatePipeWidthDialog.ui->spinBox->value();
                    mainWindow->updates();
                  });
          updatePipeWidthDialog.exec();
          return;
        }
        x += length + pointSize;
      }
      y += length + pointSize;
    }

    y = top + length + pointSize;
    for (int i = 1; i <= chip->chipSize; ++i) {
      x = lft;
      for (int j = 0; j < chip->chipSize - 1; ++j) {
        if (chip->rightPipeWidth[i][j] > 0 &&
            QRectF(x + pointSize / 2, y - pointSize / 2, length,
                   standardPipeWidth)
                .contains(e->localPos())) {
          updatePipeWidthDialog.ui->spinBox->setValue(
              chip->rightPipeWidth[i][j]);
          updatePipeWidthDialog.ui->spinBox->setMaximum(min(
              i > 1 ? adjacentLimit(chip->rightPipeWidth[i - 1][j]) : inf,
              i < chip->chipSize ? adjacentLimit(chip->rightPipeWidth[i + 1][j])
                                 : inf));
          connect(&updatePipeWidthDialog, &UpdatePipeWidthDialog::accepted,
                  [&updatePipeWidthDialog, chip, i, j, mainWindow] {
                    chip->rightPipeWidth[i][j] =
                        updatePipeWidthDialog.ui->spinBox->value();
                    mainWindow->updates();
                  });
          updatePipeWidthDialog.exec();
          return;
        }
        x += length + pointSize;
      }
      y += length + pointSize;
    }
  }
}

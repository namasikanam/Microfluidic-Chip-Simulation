#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>

class Widget : public QWidget {
  Q_OBJECT
public:
  explicit Widget(QWidget *parent = nullptr);
  ~Widget();
  void initialize();
  void process();
  void maintain();

  QList<QDoubleSpinBox *> qDoubleSpinBoxes;
  qreal downAnswer[12][12], rightAnswer[12][12];
signals:

public slots:

private:
  void paintEvent(QPaintEvent *ev);
  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);

  void debug(QDoubleSpinBox *p);
};

#endif // WIDGET_H

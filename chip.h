#ifndef CHIP_H
#define CHIP_H

#include <QtWidgets>

class Chip {
public:
  Chip();
  ~Chip();

  static Chip *getChip();
  void initialize(int _chipSize, QList<int> _inputs, QList<int> _outputs);
  bool connected();
  void process();
  void maintain(QList<qreal> ansVelocity, QList<qreal> ansDischarge,
                QList<qreal> ansConcentration);
  void calculateInputsAndOutputs();

  int chipSize;
  QList<int> inputs, outputs;
  qreal downVelocity[12][12], rightVelocity[12][12];
  qreal downDischarge[12][12], rightDischarge[12][12];
  qreal downConcentration[12][12], rightConcentration[12][12];
  // x的范围是[0,chipSize+1), y的范围是[0,chipSize)
  int downPipeWidth[12][12],
      rightPipeWidth[12][12]; // 单位是μm, 没有管子的话就是负数(因为还需要复原).
  int downID[12][12], rightID[12][12];
};

#endif // CHIP_H

#include "chip.h"
#include <bits/stdc++.h>
using namespace std;

#define forDown                                                                \
  for (int y = 0; y <= chipSize; ++y)                                          \
    for (int x = 0; x < chipSize; ++x)                                         \
      if (downID[y][x])
#define forRight                                                               \
  for (int y = 1; y <= chipSize; ++y)                                          \
    for (int x = 0; x < chipSize - 1; ++x)                                     \
      if (rightID[y][x])

static Chip *pChip;

Chip::Chip() {
  pChip = this;
  chipSize = 0;
}

Chip::~Chip() {}

Chip *Chip::getChip() { return pChip; }

void Chip::initialize(int _chipSize, QList<int> _inputs, QList<int> _outputs) {
  chipSize = _chipSize;
  inputs = _inputs;
  outputs = _outputs;

  sort(inputs.begin(), inputs.end());
  sort(outputs.begin(), outputs.end());

  for (int i = 0, j = 0; i < chipSize; ++i)
    if (j < 2 && i == inputs[j]) {
      downPipeWidth[0][i] = 200;
      ++j;
    } else
      downPipeWidth[0][i] = -200;
  for (int y = 1; y < chipSize; ++y)
    for (int x = 0; x < chipSize; ++x)
      downPipeWidth[y][x] = 200;
  for (int i = 0, j = 0; i < chipSize; ++i)
    if (j < 3 && i == outputs[j]) {
      downPipeWidth[chipSize][i] = 200;
      ++j;
    } else
      downPipeWidth[chipSize][i] = -200;

  for (int y = 1; y <= chipSize; ++y)
    for (int x = 0; x < chipSize - 1; ++x)
      rightPipeWidth[y][x] = 200;
}

void Chip::calculateInputsAndOutputs() {
  for (int x = 0, i = 0; x < chipSize; ++x)
    if (downPipeWidth[0][x] > 0)
      inputs[i++] = x;
  for (int x = 0, i = 0; x < chipSize; ++x)
    if (downPipeWidth[chipSize][x] > 0)
      outputs[i++] = x;
}

bool Chip::connected() {
  QList<int> ufs;
  for (int i = 0; i < chipSize * chipSize; ++i)
    ufs.append(i);

  auto calIndex = [this](int y, int x) { return (y - 1) * chipSize + x; };
  function<int(int)> find = [&ufs, &find](int x) {
    return x == ufs[x] ? x : ufs[x] = find(ufs[x]);
  };

  for (int y = 1; y < chipSize; ++y)
    for (int x = 0; x < chipSize; ++x)
      if (downPipeWidth[y][x] > 0)
        ufs[find(calIndex(y, x))] = find(calIndex(y + 1, x));
  for (int y = 1; y <= chipSize; ++y)
    for (int x = 0; x < chipSize - 1; ++x)
      if (rightPipeWidth[y][x] > 0)
        ufs[find(calIndex(y, x))] = find(calIndex(y, x + 1));

  calculateInputsAndOutputs();
  int tmpRoot = find(calIndex(1, inputs[0]));
  if (find(calIndex(1, inputs[1])) != tmpRoot)
    return 0;
  for (int output : outputs)
    if (find(calIndex(chipSize, output)) != tmpRoot)
      return 0;
  return 1;
}

void Chip::process() {
  auto updateMin = [](auto &a, auto b) { a = min(a, b); };
  auto updateMax = [](auto &a, auto b) { a = max(a, b); };

  const static qreal eps = 1e-9;

  // 来算一发inputs和outputs
  calculateInputsAndOutputs();

  // 假设输入流量相同
  // 这是一个(chipSize+2)*chipSize的矩阵

  qDebug() << "downPipeWidth:";
  for (int y = 0; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize; ++x)
      row += " " + QString::number(downPipeWidth[y][x]);
    qDebug() << row;
  }

  qDebug() << "rightPipeWidth:";
  for (int y = 1; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize - 1; ++x)
      row += " " + QString::number(rightPipeWidth[y][x]);
    qDebug() << row;
  }

  // 先看能到哪些点
  static bool accessed[12][12]; //点是否可达.
  memset(accessed, 0, sizeof(accessed));
  function<void(int, int)> access = [this, &access](int y, int x) {
    if (accessed[y][x])
      return;
    accessed[y][x] = 1;

    //    qDebug()<<y<<','<<x<<":"<<(y+1<=chipSize)<<' '<<(downPip)

    if (y - 1 >= 1 && downPipeWidth[y - 1][x] > 0)
      access(y - 1, x);
    if (y + 1 <= chipSize && downPipeWidth[y][x] > 0)
      access(y + 1, x);
    if (x - 1 >= 0 && rightPipeWidth[y][x - 1] > 0)
      access(y, x - 1);
    if (x + 1 < chipSize && rightPipeWidth[y][x] > 0)
      access(y, x + 1);
  };
  access(1, inputs[0]);

  qDebug() << "I can access:";
  for (int y = 1; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize; ++x)
      row += accessed[y][x] ? "1" : "0";
    qDebug() << row;
  }

  // 给亲爱的管子们编号!
  int variableTotal = 0;
  memset(rightID, 0, sizeof(rightID));
  memset(downID, 0, sizeof(downID));
  for (int y = 1; y < chipSize + 1; ++y)
    for (int x = 0; x < chipSize; ++x)
      if (accessed[y][x]) {
        if (downPipeWidth[y - 1][x] > 0 && !downID[y - 1][x])
          downID[y - 1][x] = ++variableTotal;
        if (downPipeWidth[y][x] > 0 && !downID[y][x])
          downID[y][x] = ++variableTotal;
        if (x > 0 && rightPipeWidth[y][x - 1] > 0 && !rightID[y][x - 1])
          rightID[y][x - 1] = ++variableTotal;
        if (x + 1 < chipSize && rightPipeWidth[y][x] > 0 && !rightID[y][x])
          rightID[y][x] = ++variableTotal;
      }

  qDebug() << "downID:";
  for (int y = 0; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize; ++x)
      row += " " + QString::number(downID[y][x]);
    qDebug() << row;
  }

  qDebug() << "rightID:";
  for (int y = 1; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize - 1; ++x)
      row += " " + QString::number(rightID[y][x]);
    qDebug() << row;
  }

  // Calculate discharge
  // 默认流量是从上往下, 从左往右流的
  static qreal matrix[120][120];
  memset(matrix, 0, sizeof(matrix));
  // 先来列基尔霍夫第一定律
  int rowTotal = 0;
  for (int y = 1; y <= chipSize; ++y)
    for (int x = 0; x < chipSize; ++x)
      if (accessed[y][x]) {
        ++rowTotal;
        if (downPipeWidth[y - 1][x] > 0)
          matrix[rowTotal][downID[y - 1][x]] = 1;
        if (downPipeWidth[y][x] > 0)
          matrix[rowTotal][downID[y][x]] = -1;
        if (x > 0 && rightPipeWidth[y][x - 1] > 0)
          matrix[rowTotal][rightID[y][x - 1]] = 1;
        if (x + 1 < chipSize && rightPipeWidth[y][x] > 0)
          matrix[rowTotal][rightID[y][x]] = -1;
      }

  qDebug() << "I just had fun with I.";

  // 再来找到图中所有的环
  static int stackY[100], stackX[100], stackID[100], stackWidth[100], stackTop;
  static bool inStack[10][10], edgeVisted[120];
  memset(inStack, false, sizeof(inStack));
  memset(edgeVisted, false, sizeof(edgeVisted));
  stackTop = 0;
  function<void(int, int)> findCircle = [&rowTotal, this, &findCircle](int y,
                                                                       int x) {
    //    qDebug("findCircle(%d,%d)", y, x);

    stackY[stackTop] = y;
    stackX[stackTop] = x;
    inStack[y][x] = true;

    function<void(int, int, int, int)> goTo =
        [&rowTotal, &findCircle](int y, int x, int ID, int width) {
          if (ID && !edgeVisted[ID]) {
            //            qDebug("goTo(%d,%d,%d,%d)", y, x, ID, width);
            //            qDebug("inStack[%d][%d]=%d\n", y, x, inStack[y][x]);

            edgeVisted[ID] = true;
            if (inStack[y][x]) {
              ++rowTotal;
              matrix[rowTotal][ID] = 1.0 / width;
              for (int i = stackTop; stackX[i] != x || stackY[i] != y;) {
                --i;
                matrix[rowTotal][stackID[i]] = 1.0 / stackWidth[i];
              }
            } else {
              stackID[stackTop] = ID;
              stackWidth[stackTop] = width;
              ++stackTop;
              findCircle(y, x);
              --stackTop;
            }
          }
        };

    if (y > 1)
      goTo(y - 1, x, downID[y - 1][x], -downPipeWidth[y - 1][x]);
    if (y < chipSize)
      goTo(y + 1, x, downID[y][x], downPipeWidth[y][x]);
    if (x > 0)
      goTo(y, x - 1, rightID[y][x - 1], -rightPipeWidth[y][x - 1]);
    if (x + 1 < chipSize)
      goTo(y, x + 1, rightID[y][x], rightPipeWidth[y][x]);
  };
  findCircle(1, inputs[0]);

  qDebug() << "I've just found the circles!";

  // 就这样把输出管道连起来!
  static bool pointVisited[12][12];
  function<bool(int, int, int)> findOutput =
      [this, &findOutput, &rowTotal](int y, int x, int outputX) -> bool {
    if (pointVisited[y][x])
      return false;

    pointVisited[y][x] = true;
    if (y == chipSize && x == outputX)
      return true;
    if (y > 1 && downID[y - 1][x] && findOutput(y - 1, x, outputX)) {
      matrix[rowTotal][downID[y - 1][x]] = -1.0 / downPipeWidth[y - 1][x];
      return true;
    }
    if (y < chipSize && downID[y][x] && findOutput(y + 1, x, outputX)) {
      matrix[rowTotal][downID[y][x]] = 1.0 / downPipeWidth[y][x];
      return true;
    }
    if (x > 0 && rightID[y][x - 1] && findOutput(y, x - 1, outputX)) {
      matrix[rowTotal][rightID[y][x - 1]] = -1.0 / rightPipeWidth[y][x - 1];
      return true;
    }
    if (x + 1 < chipSize && rightID[y][x] && findOutput(y, x + 1, outputX)) {
      matrix[rowTotal][rightID[y][x]] = 1.0 / rightPipeWidth[y][x];
      return true;
    }
    return false;
  };
  for (int i = 1; i <= 2; ++i) {
    //    qDebug() << "======= I will go to find " << outputs[i] << "=========";
    memset(pointVisited, false, sizeof(pointVisited));
    ++rowTotal;
    matrix[rowTotal][downID[chipSize][outputs[0]]] +=
        -1.0 / downPipeWidth[chipSize][outputs[0]];
    matrix[rowTotal][downID[chipSize][outputs[i]]] +=
        1.0 / downPipeWidth[chipSize][outputs[i]];
    findOutput(chipSize, outputs[0], outputs[i]);
  }
  // 不要忘记设置输入流量!
  for (int i = 0; i < 2; ++i) {
    matrix[++rowTotal][downID[0][inputs[i]]] = 1;
    matrix[rowTotal][variableTotal + 1] = 1;
  }
  // Show my martrix
  //  qDebug() << "Below is my matrix.";
  //  for (int i = 1; i <= rowTotal; ++i) {
  //    QString row;
  //    for (int j = 1; j <= variableTotal + 1; ++j)
  //      row += " " + QString::number(matrix[i][j]);
  //    qDebug() << row;
  //  }
  // 可以来算高斯消元啦!
  // 方程会不会无解/多解呢?
  for (int i = 1, j = 1; i <= variableTotal; ++i) {
    {
      int k = j;
      while (k <= rowTotal && abs(matrix[k][i]) < eps)
        ++k;
      if (k > rowTotal)
        continue;
      else
        swap(matrix[j], matrix[k]);
    }
    for (int k = j + 1; k <= rowTotal; ++k)
      if (abs(matrix[k][i]) > eps) {
        qreal tmp = matrix[k][i] / matrix[j][i];
        for (int o = i; o <= variableTotal + 1; ++o)
          matrix[k][o] -= matrix[j][o] * tmp;
      }
    ++j;
  }

  //  qDebug() << "Shang San Jiao Ju Zhen:";
  //  for (int i = 1; i <= rowTotal; ++i) {
  //    QString row;
  //    for (int j = 1; j <= variableTotal + 1; ++j)
  //      row += " " + QString::number(matrix[i][j], 'f', 0);
  //    qDebug() << row;
  //  }

  for (int i = rowTotal; i; --i) {
    matrix[i][variableTotal + 1] /= matrix[i][i];
    matrix[i][i] = 1;
    for (int j = i - 1; j; --j) {
      matrix[j][variableTotal + 1] -=
          matrix[i][variableTotal + 1] * matrix[j][i];
      matrix[j][i] = 0;
    }
  }
  // 给大家发答案啦!
  qreal maxDischarge = 0;
  for (int y = 0; y <= chipSize; ++y)
    for (int x = 0; x < chipSize; ++x)
      if (downID[y][x])
        maxDischarge =
            max(maxDischarge,
                downDischarge[y][x] = matrix[downID[y][x]][variableTotal + 1]);
  for (int y = 1; y <= chipSize; ++y)
    for (int x = 0; x + 1 < chipSize; ++x)
      if (rightID[y][x])
        maxDischarge =
            max(maxDischarge, rightDischarge[y][x] =
                                  matrix[rightID[y][x]][variableTotal + 1]);
  for (int y = 0; y <= chipSize; ++y)
    for (int x = 0; x < chipSize; ++x)
      if (downID[y][x])
        downDischarge[y][x] /= maxDischarge;
  for (int y = 1; y <= chipSize; ++y)
    for (int x = 0; x < chipSize - 1; ++x)
      if (rightID[y][x])
        rightDischarge[y][x] /= maxDischarge;

  // 计算流速的话, 需要假设最大的流速为1, 一个简单的normalize的操作.
  qreal maxVelocity = 0;
  forDown updateMax(maxVelocity, downVelocity[y][x] =
                                     matrix[downID[y][x]][variableTotal + 1] /
                                     downPipeWidth[y][x]);
  forRight updateMax(maxVelocity, rightVelocity[y][x] =
                                      matrix[rightID[y][x]][variableTotal + 1] /
                                      rightPipeWidth[y][x]);
  forDown downVelocity[y][x] /= maxVelocity;
  forRight rightVelocity[y][x] /= maxVelocity;

  qDebug() << "downVelocity";
  for (int y = 0; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize; ++x)
      row += " " + QString::number(downVelocity[y][x], 'f', 2);
    qDebug() << row;
  }
  qDebug() << "rightVelocity";
  for (int y = 1; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize - 1; ++x)
      row += " " + QString::number(rightVelocity[y][x], 'f', 2);
    qDebug() << row;
  }

  // 现在来计算浓度.
  static int degree[12][12];
  memset(degree, 0, sizeof(degree));
  memset(downConcentration, 0, sizeof(downConcentration));
  memset(rightConcentration, 0, sizeof(rightConcentration));
  forDown if (abs(downDischarge[y][x]) > eps) {
    if (downDischarge[y][x] > 0)
      ++degree[y + 1][x];
    else
      ++degree[y][x];
  }
  forRight if (abs(rightDischarge[y][x]) > eps) {
    if (rightDischarge[y][x] > 0)
      ++degree[y][x + 1];
    else
      ++degree[y][x];
  }

  function<void(int, int)> calConcentration = [this, &calConcentration](int y,
                                                                        int x) {
    //    qDebug("calConcentration(%d,%d)", y, x);

    // 构建
    QList<qreal> inputConcentration, outputConcentration;
    QList<qreal> inputDischarge, outputDischarge;
    QList<int> inputDirection, outputDirection;
    if (x + 1 < chipSize && rightID[y][x] && abs(rightDischarge[y][x]) > eps) {
      if (rightDischarge[y][x] < 0) {
        inputDischarge.append(-rightDischarge[y][x]);
        inputDirection.append(0);
        inputConcentration.append(rightConcentration[y][x]);
      } else {
        outputDischarge.append(rightDischarge[y][x]);
        outputDirection.append(0);
        outputConcentration.append(0);
      }
    }
    if (y - 1 >= 0 && downID[y - 1][x] && abs(downDischarge[y - 1][x]) > eps) {
      if (downDischarge[y - 1][x] > 0) {
        inputDischarge.append(downDischarge[y - 1][x]);
        inputDirection.append(1);
        inputConcentration.append(downConcentration[y - 1][x]);
      } else {
        outputDischarge.append(-downDischarge[y - 1][x]);
        outputDirection.append(1);
        outputConcentration.append(0);
      }
    }
    if (x - 1 >= 0 && rightID[y][x - 1] &&
        abs(rightDischarge[y][x - 1]) > eps) {
      if (rightDischarge[y][x - 1] > 0) {
        inputDischarge.append(rightDischarge[y][x - 1]);
        inputDirection.append(2);
        inputConcentration.append(rightConcentration[y][x - 1]);
      } else {
        outputDischarge.append(-rightDischarge[y][x - 1]);
        outputDirection.append(2);
        outputConcentration.append(0);
      }
    }
    if (y <= chipSize && downID[y][x] && abs(downDischarge[y][x]) > eps) {
      if (downDischarge[y][x] < 0) {
        inputDischarge.append(-downDischarge[y][x]);
        inputDirection.append(3);
        inputConcentration.append(downConcentration[y][x]);
      } else {
        outputDischarge.append(downDischarge[y][x]);
        outputDirection.append(3);
        outputConcentration.append(0);
      }
    }

    // 计算
    //    qDebug("inDegree=%d,outDegree=%d", inputDischarge.length(),
    //           outputDischarge.length());
    //    for (int iD : inputDirection)
    //      qDebug("iD: %d", iD);
    //    for (int oD : outputDirection)
    //      qDebug("oD: %d", oD);

    if (inputDischarge.length() == 1) {
      for (int i = outputDischarge.length(); i--;)
        outputConcentration[i] = inputConcentration[0];
    } else if (outputDischarge.length() == 1) {
      qreal sum = 0;
      for (int i = inputDischarge.length(); i--;)
        sum += inputDischarge[i] * inputConcentration[i];
      outputConcentration[0] = sum / outputDischarge[0];
    } else {
      for (int i = 2; i--;)
        for (int j = 2; j--;)
          if (inputDischarge[i] > outputDischarge[j] &&
              (inputDirection[i] - outputDirection[j] & 1) == 1) {
            outputConcentration[j] = inputConcentration[i];
            outputConcentration[j ^ 1] =
                (inputConcentration[0] * inputDischarge[0] +
                 inputConcentration[1] * inputDischarge[1] -
                 outputConcentration[j] * outputDischarge[j]) /
                outputDischarge[j ^ 1];
          }

      //      for (int i = 2; i--;)
      //        qDebug("%d: inputDischarge:%.2lf, inputConcentration:%.2lf", i,
      //               inputDischarge[i], inputConcentration[i]);
      //      qDebug("inputSum=%.6lf", inputConcentration[0] * inputDischarge[0]
      //      +
      //                                   inputConcentration[1] *
      //                                   inputDischarge[1]);
      //      for (int i = 2; i--;)
      //        qDebug("%d: outputDischarge:%.2lf, outputConcentration:%.2lf",
      //        i,
      //               outputDischarge[i], outputConcentration[i]);
      //      qDebug("outputSum=%.6lf",
      //             outputConcentration[0] * outputDischarge[0] +
      //                 outputConcentration[1] * outputDischarge[1]);
    }

    // 分发浓度
    for (int i = outputDischarge.length(); i--;)
      switch (outputDirection[i]) {
      case 0:
        rightConcentration[y][x] = outputConcentration[i];
        break;
      case 1:
        downConcentration[y - 1][x] = outputConcentration[i];
        break;
      case 2:
        rightConcentration[y][x - 1] = outputConcentration[i];
        break;
      case 3:
        downConcentration[y][x] = outputConcentration[i];
        break;
      }

    // 减度数
    for (int i = outputDischarge.length(); i--;)
      switch (outputDirection[i]) {
      case 0:
        if (--degree[y][x + 1] == 0)
          calConcentration(y, x + 1);
        break;
      case 1:
        if (--degree[y - 1][x] == 0)
          calConcentration(y - 1, x);
        break;
      case 2:
        if (--degree[y][x - 1] == 0)
          calConcentration(y, x - 1);
        break;
      case 3:
        if (y < chipSize && --degree[y + 1][x] == 0)
          calConcentration(y + 1, x);
        break;
      }
  };
  downConcentration[0][inputs[0]] = 1;
  downConcentration[0][inputs[1]] = 0;
  for (int i = 0; i < 2; ++i)
    if (--degree[1][inputs[i]] == 0)
      calConcentration(1, inputs[i]);

  qreal maxConCentration = 0;
  forDown updateMax(maxConCentration, downConcentration[y][x]);
  forRight updateMax(maxConCentration, rightConcentration[y][x]);
  forDown downConcentration[y][x] /= maxConCentration;
  forRight downConcentration[y][x] /= maxConCentration;

  qDebug() << "downConcentration";
  for (int y = 0; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize; ++x)
      row += " " + QString::number(downConcentration[y][x], 'f', 2);
    qDebug() << row;
  }
  qDebug() << "rightConcentration";
  for (int y = 1; y <= chipSize; ++y) {
    QString row;
    for (int x = 0; x < chipSize - 1; ++x)
      row += " " + QString::number(rightConcentration[y][x], 'f', 2);
    qDebug() << row;
  }
}

void Chip::maintain(QList<qreal> ansVelocity, QList<qreal> ansDischarge,
                    QList<qreal> ansConcentration) {
  auto calLoss = [&]() -> qreal {
    double loss = 0;
    for (int i = 0; i < 3; ++i)
      loss +=
          pow(ansVelocity[i] - downVelocity[chipSize][outputs[i]], 2) +
          pow(ansDischarge[i] - downDischarge[chipSize][outputs[i]], 2) +
          pow(ansConcentration[i] - downConcentration[chipSize][outputs[i]], 2);
    return loss;
  };
  auto updateMin = [](auto &a, auto b) { a = min(a, b); };
  auto adjacentLimit = [](int width) -> int {
    return (1600 + 100 * 2 - ((width < 0 ? 20 : width) + 1) / 2 - 200) * 2;
  };

  qDebug("Start maintain in chip");

  qreal loss = calLoss();
  const int randTotal = 50;
  const int inf = 10000;
  for (int T = 100; T--;) {
    [&]() {
      // 修改某个管道的宽度
      // 先就写个这个看看...
      forDown if (rand() % randTotal == 0) {
        int maxWidth =
                min(x > 0 ? adjacentLimit(downPipeWidth[y][x - 1]) : inf,
                    x + 1 < chipSize ? adjacentLimit(downPipeWidth[y][x + 1])
                                     : inf),
            tmpWidth = downPipeWidth[y][x];
        downPipeWidth[y][x] = 20 + rand() % (maxWidth - 20 + 1);

        qDebug("Now I change downPipeWidth[%d][%d] -> %d", y, x,
               downPipeWidth[y][x]);

        process();
        if (calLoss() < loss)
          loss = calLoss();
        else
          downPipeWidth[y][x] = tmpWidth;
      }
      forRight if (rand() % randTotal == 0) {
        int maxWidth =
                min(y > 1 ? adjacentLimit(rightPipeWidth[y - 1][x]) : inf,
                    y + 1 <= chipSize ? adjacentLimit(rightPipeWidth[y + 1][x])
                                      : inf),
            tmpWidth = rightPipeWidth[y][x];
        rightPipeWidth[y][x] = 20 + rand() % (maxWidth - 20 + 1);

        qDebug("Now I change rightPipeWidth[%d][%d] -> %d", y, x,
               rightPipeWidth[y][x]);

        process();
        if (calLoss() < loss)
          loss = calLoss();
        else
          rightPipeWidth[y][x] = tmpWidth;
      }

      // 添加一个管道
      // 删除一个管道
    }();
  }
  process();
}

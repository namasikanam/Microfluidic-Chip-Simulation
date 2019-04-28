#include "mainwindow.h"
#include "proxystyle.h"
#include <QApplication>
#include <QProxyStyle>
#include <QtWidgets>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // Apply Style
  QFile f(":qdarkstyle/style.qss");
  f.open(QFile::ReadOnly | QFile::Text);
  QTextStream ts(&f);
  qApp->setStyleSheet(ts.readAll());

  // set underline shortcut style in any platform
  a.setStyle(new ProxyStyle);

  MainWindow w;
  w.show();

  return a.exec();
}

#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QProxyStyle>
#include <QtWidgets>

class ProxyStyle : public QProxyStyle {
public:
  ProxyStyle();
  ~ProxyStyle();

  int styleHint(QStyle::StyleHint hint, const QStyleOption *option,
                const QWidget *widget, QStyleHintReturn *returnData) const;
};

#endif // PROXYSTYLE_H

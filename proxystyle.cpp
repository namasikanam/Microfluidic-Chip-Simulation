#include "proxystyle.h"

ProxyStyle::ProxyStyle() {}

ProxyStyle::~ProxyStyle() {}

int ProxyStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option,
                          const QWidget *widget,
                          QStyleHintReturn *returnData) const {
  if (hint == QStyle::SH_UnderlineShortcut)
    return 1;
  else
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

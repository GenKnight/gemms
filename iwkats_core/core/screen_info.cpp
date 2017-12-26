#include "screen_info.h"

#include <QStyle>
#include <QQmlContext>
#include <QQmlEngine>
#include <QApplication>
#include <QDesktopWidget>
#include <QQuickWindow>

#include "logging.h"

IWKATS_NAMESPACE_BEGIN

ScreenInfo::ScreenInfo()
{
  /// 获取屏幕大小
  QDesktopWidget *deskWgt = QApplication::desktop();
  if (deskWgt) {
    virtualWidth = deskWgt->width();
    virtualHeight = deskWgt->height();
//    LOG_INFO << "virtual width:" << virtualWidth << ",height:" << virtualHeight;

    QRect availableRect = deskWgt->availableGeometry();
    availableWidth = availableRect.width();
    availableHeight = availableRect.height();
//    LOG_INFO << "available width:" <<availableWidth << ",height:" << availableHeight;

    auto availableTitleHeight = QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight);
    availableHeight -= availableTitleHeight;
//    LOG_INFO << "available width:" <<availableWidth << ",height:" << availableHeight;

    QRect screenRect = deskWgt->screenGeometry();
    screenWidth = screenRect.width();
    screenHeight = screenRect.height();
//    LOG_INFO << "screen width:" <<screenWidth << ",height:" << screenHeight;
  }
}

void ScreenInfo::bringWindowToTop(QQmlApplicationEngine* engine)
{
  QObject* object = engine->rootObjects().first();
  QQuickWindow* autoQuoteWindow = qobject_cast<QQuickWindow*>(object);
  //bring window to top on OSX
  autoQuoteWindow->show();
  //bring window from minimized state on OSX
  autoQuoteWindow->raise();
  //bring window to front/unminimize on windows
  autoQuoteWindow->requestActivate();
}

IWKATS_NAMESPACE_END

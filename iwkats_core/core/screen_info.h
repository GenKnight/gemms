#pragma once

#include "core/iwkats_core.h"
#include <QObject>
#include <QQmlApplicationEngine>

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT ScreenInfo
{
public:
  ScreenInfo();

  static void bringWindowToTop(QQmlApplicationEngine* engine);

public:
  int virtualWidth = 0;
  int virtualHeight = 0;
  int availableWidth = 0;
  int availableHeight = 0;
  int screenWidth = 0;
  int screenHeight = 0;
};

IWKATS_NAMESPACE_END


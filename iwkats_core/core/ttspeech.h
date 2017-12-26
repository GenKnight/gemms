#pragma once

#include "core/iwkats_core.h"

#include <QVoice>
#include <QLocale>
#include <QTextToSpeech>

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT TTSpeech {
  TTSpeech();
public:
  static TTSpeech* GetPtr();

  void setVoice(const QVoice& voice);

  void say(const QString& text);

private:
  struct Private;
  std::shared_ptr<Private> d_;
};

IWKATS_NAMESPACE_END

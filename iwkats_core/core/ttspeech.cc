#include "ttspeech.h"

#include <mutex>

IWKATS_NAMESPACE_BEGIN

struct TTSpeech::Private {
  std::mutex mutex;
  QTextToSpeech speech;
};

TTSpeech::TTSpeech()
  : d_(new Private) {

  d_->speech.setLocale(QLocale(QLocale::Chinese, QLocale::China));
  d_->speech.setRate(0.3);
}

TTSpeech*TTSpeech::GetPtr()
{
  static TTSpeech instance;
  return &instance;
}

void TTSpeech::setVoice(const QVoice& voice)
{
  d_->speech.setVoice(voice);
}

void TTSpeech::say(const QString& text)
{
  std::lock_guard<std::mutex> lock(d_->mutex);
  Q_UNUSED(lock);
  d_->speech.say(text);
}

IWKATS_NAMESPACE_END

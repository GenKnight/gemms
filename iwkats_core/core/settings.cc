#include "settings.h"

IWKATS_NAMESPACE_BEGIN

Settings::Settings(QObject *parent)
  : QSettings(parent)
{

}

Settings::~Settings()
{

}

void Settings::setValue(const QString &key, const QVariant &value)
{
  QSettings::setValue(key, value);
}

QVariant Settings::value(const QString &key, const QVariant &defalutValue) const
{
  return QSettings::value(key, defalutValue);
}


IWKATS_NAMESPACE_END

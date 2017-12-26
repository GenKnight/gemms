#pragma once

#include <core/iwkats_core.h>

#include <QSettings>

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT Settings : public QSettings
{
    Q_OBJECT
public:
    Settings(QObject* parent = Q_NULLPTR);
    virtual ~Settings();

    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defalutValue = QVariant()) const;
};

IWKATS_NAMESPACE_END

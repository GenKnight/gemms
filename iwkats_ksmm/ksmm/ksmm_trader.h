#pragma once

#include "core/iwkats_core.h"

#include <QObject>

#include "ksmm/ksmm_trading_api.h"

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT Trader : public QObject {
  Q_OBJECT
  explicit Trader(QObject* parent = nullptr);
public:
  static Trader* GetPtr();
  static KSMMTradingSpi* Spi();
  static KSMMTradingApi* Api();

public slots:
  void slotLoginResult(bool result);

signals:
  void loginResult(bool);

private:
  KSMMTradingSpi* _Spi() const;
  KSMMTradingApi* _Api() const;

  void loginSuccess();

private:
  struct Private;
  std::shared_ptr<Private> d_;
};

IWKATS_NAMESPACE_END

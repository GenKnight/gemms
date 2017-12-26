#include "ksmm_trader.h"

#include <chrono>

#include "ksmm/common_define.hpp"
#include "ksmm/ksmm_trading_api.h"
#include "ksmm/market/market_manager.h"
#include "ksmm/match/match_manager.h"

#include "order/order_manager.h"

IWKATS_NAMESPACE_BEGIN

struct Trader::Private {
  KSMMTradingApi* api;
};

//////////////////////////////////////////////////////////////////////////

Trader::Trader(QObject* parent /* = nullptr */)
  : QObject(parent)
  , d_(new Private) {

  d_->api = new KSMMTradingApi(this);
  connect(d_->api->Spi(), &KSMMTradingSpi::loginResult, this, &Trader::slotLoginResult);
}

Trader* Trader::GetPtr() {
  static Trader instance;
  return &instance;
}

KSMMTradingSpi* Trader::Spi() {
  return Trader::GetPtr()->_Spi();
}

KSMMTradingSpi* Trader::_Spi() const {
  return d_->api->Spi();
}

KSMMTradingApi* Trader::Api()
{
  return Trader::GetPtr()->_Api();
}

void Trader::slotLoginResult(bool result)
{
  if(result) {
    loginSuccess();
  }

  emit loginResult(result);
}

KSMMTradingApi* Trader::_Api() const
{
  return d_->api;
}

void Trader::loginSuccess()
{
  // 初始化行情管理器
  MarketManager::GetPtr();

  // 初始化成交管理器
  MatchManager::GetPtr();

  // 初始化委托管理器
  OrderManager::GetPtr();

  // 订阅所有股票行情
  std::vector<ASSET_CODE*> codes;
  Trader::Spi()->GetAssetCodes(codes);
  for(const auto& asset_code : codes) {
    Trader::Api()->subscribe(asset_code->szStkCode);
  }

}


IWKATS_NAMESPACE_END

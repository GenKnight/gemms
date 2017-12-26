#include "ksmm_trading_api.h"

#include <QThread>
#include <QReadLocker>

#include "ksmm_trading_spi.h"
#include "ksmm/market/market_manager.h"

IWKATS_NAMESPACE_BEGIN

struct KSMMTradingApi::Private {

  QThread spi_thread;
  KSMMTradingSpi* spi;
};

//////////////////////////////////////////////////////////////////////////

KSMMTradingApi::KSMMTradingApi(QObject* parent /* = nullptr */)
  : QObject(parent)
  , d_(new Private) {

  // api 单独一个线程
  d_->spi = new KSMMTradingSpi();
  d_->spi->moveToThread(&d_->spi_thread);

  /// 关联信号和槽
  connect(&d_->spi_thread, &QThread::finished, d_->spi, &QObject::deleteLater);

  // 登陆
  connect(this, &KSMMTradingApi::requestLogin, d_->spi, &KSMMTradingSpi::Login);
  // 请求报价
  connect(this, &KSMMTradingApi::quote, d_->spi, &KSMMTradingSpi::quote);
  connect(this, &KSMMTradingApi::algoQuote, d_->spi, &KSMMTradingSpi::algoQuote);
  // 撤单
  connect(this, &KSMMTradingApi::cancelOrder, d_->spi, &KSMMTradingSpi::cancelOrder);
  // 去操作员资金
  connect(this, &KSMMTradingApi::getFunds, d_->spi, &KSMMTradingSpi::getFunds);
  // 取操作员的持仓
  connect(this, &KSMMTradingApi::getShare, d_->spi, &KSMMTradingSpi::getShare);
  // 查询做市交易委托单
  connect(this, &KSMMTradingApi::listQuote, d_->spi, &KSMMTradingSpi::listQuote);
  // 查询成交
  connect(this, &KSMMTradingApi::listQuoteMatch, d_->spi, &KSMMTradingSpi::listQuoteMatch);
  // 查询资产
  connect(this, &KSMMTradingApi::queryAssetFund, d_->spi, &KSMMTradingSpi::queryAssetFund);
  // 取操作员的持仓
  connect(this, &KSMMTradingApi::queryAssetShare, d_->spi, &KSMMTradingSpi::queryAssetShare);

  // 子线程运行
  d_->spi_thread.start();
}

KSMMTradingApi::~KSMMTradingApi() {
  d_->spi_thread.quit();
  d_->spi_thread.wait();
}

KSMMTradingSpi* KSMMTradingApi::Spi() const {
  return d_->spi;
}

int KSMMTradingApi::subscribe(const QString& code)
{
  return MarketManager::GetPtr()->subscribe(code);
}

int KSMMTradingApi::unsubscribe(const QString& code)
{
  return MarketManager::GetPtr()->unsubscribe(code);
}

IWKATS_NAMESPACE_END

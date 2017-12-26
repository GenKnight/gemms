#include "market_manager.h"

#include <QHash>
#include <QReadWriteLock>

#include "ksmm/ksmm_trader.h"

IWKATS_NAMESPACE_BEGIN

struct MarketManager::Private {

  QReadWriteLock rwlock;
  QHash<QString, int> stock_subscribes;
  QHash<QString, std::shared_ptr<STK_NQ_TICK>> stocks_nq_tick;
  QHash<QString, std::shared_ptr<STK_NQ_ZSXX>> stocks_zs_tick;
};

//////////////////////////////////////////////////////////////////////////

MarketManager::MarketManager(QObject *parent)
  : QObject(parent)
  , d_(new Private) {


  connect(Trader::Spi(), &KSMMTradingSpi::sigNqTickArrive, this, &MarketManager::slotNqTickArrive);
  connect(Trader::Spi(), &KSMMTradingSpi::sigZsTickArrive, this, &MarketManager::slotZsTickArrive);
}

MarketManager*MarketManager::GetPtr()
{
  static MarketManager instance;
  return &instance;
}

int MarketManager::subscribe(const QString& stock)
{
  QWriteLocker wlock(&d_->rwlock);
  auto preCount = d_->stock_subscribes.value(stock, 0);
  if(0 == preCount) {
    Trader::Spi()->subscribe(stock);
  } else {
    emitCurrentCacheTick(stock);
  }
  d_->stock_subscribes.insert(stock, ++preCount);
  return 0;
}

int MarketManager::unsubscribe(const QString& stock)
{
  QWriteLocker wlock(&d_->rwlock);
  auto preCount = d_->stock_subscribes.value(stock, 0);
  if(preCount <= 1) {
    Trader::Spi()->unsubscribe(stock);
    // 从缓冲中移除该股票
    d_->stock_subscribes.remove(stock);
    d_->stocks_nq_tick.remove(stock);
    d_->stocks_zs_tick.remove(stock);
  } else {
    d_->stock_subscribes[stock] = --preCount;
  }
  return 0;
}

std::shared_ptr<STK_NQ_TICK> MarketManager::GetStockNQTick(const QString& stock)
{
  // step 1 是否已经订阅
  {
    // 如果有当前股票订阅，直接返回
    QReadLocker rlock(&d_->rwlock);
    if(d_->stocks_nq_tick.contains(stock)) {
      return d_->stocks_nq_tick.value(stock);
    }

    // 查看是否已经订阅
    if(d_->stock_subscribes.contains(stock)) {
      return nullptr;
    }
  }

  // step 2 还没订阅，先订阅行情
  subscribe(stock);
  return nullptr;
}

std::shared_ptr<STK_NQ_ZSXX> MarketManager::GetStockZSTick(const QString& stock)
{
  // step 1 是否已经订阅
  {
    // 如果有当前股票订阅，直接返回
    QReadLocker rlock(&d_->rwlock);
    if(d_->stocks_zs_tick.contains(stock)) {
      return d_->stocks_zs_tick.value(stock);
    }

    // 查看是否已经订阅
    if(d_->stock_subscribes.contains(stock)) {
      return nullptr;
    }
  }

  // step 2 还没订阅，先订阅行情
  subscribe(stock);
  return nullptr;
}

void MarketManager::slotNqTickArrive(std::shared_ptr<STK_NQ_TICK> pNQHQInfo)
{
  QWriteLocker wlock(&d_->rwlock);
  d_->stocks_nq_tick[pNQHQInfo->szStkCode] = pNQHQInfo;
  emit sigNqTickArrive(pNQHQInfo);
}

void MarketManager::slotZsTickArrive(std::shared_ptr<STK_NQ_ZSXX> pZSHQInfo)
{
  QWriteLocker wlock(&d_->rwlock);
  d_->stocks_zs_tick[pZSHQInfo->szStkCode] = pZSHQInfo;
  emit sigZsTickArrive(pZSHQInfo);
}

void MarketManager::emitCurrentCacheTick(const QString& stock)
{
  // 如果有缓存得行情，发送
  if(d_->stocks_nq_tick.contains(stock)) {
    emit sigNqTickArrive(d_->stocks_nq_tick[stock]);
  }
  if(d_->stocks_zs_tick.contains(stock)) {
    emit sigZsTickArrive(d_->stocks_zs_tick[stock]);
  }
}

IWKATS_NAMESPACE_END

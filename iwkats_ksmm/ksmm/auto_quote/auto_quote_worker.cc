#include "auto_quote_worker.h"

#include <QTimer>
#include <QVariant>
#include <QSettings>

#include "core/util.hpp"
#include "core/ttspeech.h"

#include "ksmm/stock_object.h"
#include "ksmm/order/order_info.h"
#include "ksmm/market/market_manager.h"

IWKATS_NAMESPACE_BEGIN

struct AutoQuoteWorker::Private {
  QString stock;
  QTimer* timer;
  bool running{false};
  int auto_quote_interval{120};

  // 标识委托回报是否是策略主动委托
  bool is_auto_quote{true};

  // 标识撤单回报是否是策略主动撤单
  bool is_auto_canceled{false};

  // 是否需要暂停策略执行，如果遇到有手动委托
  bool is_pause{false};

  // 委托是否是自动重复报价的委托， 用于停止自动重复报价时，是否撤销该笔委托用
  bool is_auto_quote_order{false};

  std::shared_ptr<StockObject> stock_object{nullptr};
};

//////////////////////////////////////////////////////////////////////////

AutoQuoteWorker::AutoQuoteWorker(const QString& stock, 
                                 QObject* parent /* = nullptr */)
  : QObject(parent)
  , d_(new Private) {

  d_->stock = stock;
  d_->timer = new QTimer(this);

  d_->stock_object = std::make_shared<StockObject>();
  d_->stock_object->setStock(stock);
  d_->stock_object->setType(StockObject::QUOTE_BY_AUTO_QUOTE);

  connect(MarketManager::GetPtr(), &MarketManager::sigZsTickArrive, this, &AutoQuoteWorker::slotZsTickArrive);

  connect(d_->timer, &QTimer::timeout, this, &AutoQuoteWorker::autoQuoteWaitTimeout);

  connect(d_->stock_object.get(), &StockObject::appendLog, this, &AutoQuoteWorker::slotStockAppendLog);
  connect(d_->stock_object.get(), &StockObject::sigStockQuoteError, this, &AutoQuoteWorker::sigStockQuoteError);

  // 委托状态
  connect(d_->stock_object.get(), &StockObject::orderInfoChanged, this, &AutoQuoteWorker::sigOrderInfoChanged);
  connect(d_->stock_object.get(), &StockObject::sigOrderCreated, this, &AutoQuoteWorker::slotOrderCreated);
  connect(d_->stock_object.get(), &StockObject::sigOrderMatched, this, &AutoQuoteWorker::slotOrderMatched);
  connect(d_->stock_object.get(), &StockObject::sigOrderCanceled, this, &AutoQuoteWorker::slotOrderCanceled);

  // 获取默认
  QSettings settings;
  QString intervalKey = stock + "," + "autoQuoteInterval";
  d_->auto_quote_interval = settings.value(intervalKey, 120).toInt();
}

QString AutoQuoteWorker::stock() const
{
  return d_->stock;
}

QString AutoQuoteWorker::speechStock() const {
  return d_->stock_object->speechStock();
}

QString AutoQuoteWorker::speechStockName() const
{
  return d_->stock_object->stockName();
}

bool AutoQuoteWorker::isRunning() const
{
  return d_->running;
}

bool AutoQuoteWorker::isWait() const
{
  return d_->timer->isActive();
}

bool AutoQuoteWorker::isPause() const
{
  return d_->is_pause;
}

StockObject*AutoQuoteWorker::stockObject() const
{
  return d_->stock_object.get();
}

void AutoQuoteWorker::startAutoQuote()
{
  d_->running = true;
  emit sigAutoQuoteStarted();

  // 判断当前是否手动委托或者撤单，需要等待
  if(!_CheckNeedPauseWhenStart()) {
    const auto& pZSHQInfo = MarketManager::GetPtr()->GetStockZSTick(stock());
    if(pZSHQInfo) {
      slotZsTickArrive(pZSHQInfo);
    }
  }
}

void AutoQuoteWorker::stopAutoQuote()
{
  if(isRunning()) {
    d_->timer->stop();
    d_->running = false;
    d_->is_pause = false;
    d_->is_auto_quote = false;
    d_->is_auto_canceled = false;

    cancelOrder();
    emit sigAutoQuoteStoped();
  }
}


void AutoQuoteWorker::slotOrderCreated() {
  d_->is_auto_quote_order = false;

  if(!isRunning()) {
    return;
  }

  // 如果是手动下单，开启等待
  if(!isAutoQuote()) {
    // 暂停策略执行，开启等待
    d_->is_pause = true;
    return;
  }

  // 自动报价成功
  const auto& pOrderInfo = stockObject()->orderInfo()->GetOrderInfoPtr();
  TTSpeech::GetPtr()->say(
        speechStockName() + speechStock()
        + QString::fromLocal8Bit(",自动重复报价成功,叫卖价: ") + QString::number(pOrderInfo->dAskPrice)
        + QString::fromLocal8Bit(",叫买价: ") + QString::number(pOrderInfo->dBidPrice)
        );
  d_->is_auto_quote_order = true;
  d_->is_auto_quote = false;
}

void AutoQuoteWorker::slotOrderMatched() {
  if(!isRunning()) {
    return;
  }

  // 成交开启等待
  startWaitTimer();
}

void AutoQuoteWorker::slotOrderCanceled() {
  if(!isRunning()) {
    return;
  }

  if(!isAutoCanceled()) {
    // 手动撤单，暂停结束，开启等待
    d_->is_pause = false;
    startWaitTimer();
    return;
  }

  d_->is_auto_canceled = false;
}


void AutoQuoteWorker::slotZsTickArrive(std::shared_ptr<STK_NQ_ZSXX> pZSHQInfo)
{
  if(!isRunning() || 0 != strcmp(stock().toStdString().data(), pZSHQInfo->szStkCode)) {
    return;
  }

  if(isWait()) {
    std::string log_msg = QString::number(d_->timer->remainingTime() / 1000 * 1.0).toStdString() + "s 后尝试自动报单";
    emit sigAutoQuoteLog(QString::fromLocal8Bit(log_msg.data()));
    return;
  }

  if(isPause()) {
    return;
  }

  bool bCanQuote{true};
  double dBidPrice1 = 0.0;
  double dAskPrice1 = 0.0;
  do {
    // 当前做市买1
    dBidPrice1 = pZSHQInfo->dMMDishPri[Buy][0];

    // 当前做市行情卖1价 (卖1-卖10不为0得第一个价即为卖1价)
    for(auto i = 9; i >= 0; --i) {
      auto currAskPrice = pZSHQInfo->dMMDishPri[Sell][i];
      if(0.0 != currAskPrice) {
        dAskPrice1 = currAskPrice;
        break;
      }
    }

    if(0.0 == dAskPrice1 || 0.0 == dBidPrice1) {
      bCanQuote = false;
      break;
    }

  } while(0);

  if(bCanQuote) {
    _AutoQuote(dAskPrice1, dBidPrice1);
  }
}

void AutoQuoteWorker::autoQuoteWaitTimeout()
{
  // 当前策略是否已经暂停或停止
  if(!isRunning() || isPause()) {
    return;
  }

  // 等待结束, 获取当前行情继续报单
  const auto& pZSHQInfo = MarketManager::GetPtr()->GetStockZSTick(stock());
  if(pZSHQInfo) {
    slotZsTickArrive(pZSHQInfo);
  }
}

void AutoQuoteWorker::slotStockAppendLog(const QString& msg) {
  if(isRunning()) {
    emit sigAutoQuoteLog(msg);
  }
}

bool AutoQuoteWorker::startWaitTimer()
{
  if(!isRunning()) {
    return false;
  }

  // 重启定时器等待
  d_->auto_quote_interval = getAutoQuoteInterval();
  d_->timer->start(d_->auto_quote_interval * 1000);
  d_->timer->setSingleShot(true);
  return true;
}


void AutoQuoteWorker::_AutoQuote(double dAskPrice1, double dBidPrice1, int nAskVol, int nBidVol)
{
  // 当前可以买卖可以变动的最大区间
  auto dAskBidMaximumSpread = (dAskPrice1 - (dAskPrice1 * (1 - kMaximumAskBidRate))) / 2;
  auto dSuitableAskPrice = dAskPrice1 + dAskBidMaximumSpread;
  auto dSuitableBidPrice = dBidPrice1 - dAskBidMaximumSpread;

  // 先往大调，卖价向上取整，买价向下取整
  // 保留2位小数，不四舍五入
  // 卖价向上取整
  dSuitableAskPrice = ceil(dSuitableAskPrice * 100) / 100;

  // 买价向下取整
  dSuitableBidPrice = floor(dSuitableBidPrice * 100) / 100;

  LOG_INFO << stock().toStdString()
           << " First auto quote suitable ask bid range["
           << dSuitableAskPrice << ", " << dSuitableBidPrice
           << "]";

  // 检查算的价格是否符合价差5%
  double currAskBidRate = (dSuitableAskPrice - dSuitableBidPrice) / dSuitableAskPrice;
  auto totalCount = 0;
  while(currAskBidRate > kMaximumAskBidRate) {
    if(0 == (++totalCount % 2)) {
      // 微调卖价
      dSuitableAskPrice -= kMinimumQuoteSpread;

    } else {
      // 微调买价
      dSuitableBidPrice += kMinimumQuoteSpread;
    }
    currAskBidRate = (dSuitableAskPrice - dSuitableBidPrice) / dSuitableAskPrice;
  }

  LOG_INFO << stock().toStdString()
           << " Final auto quote suitable price ["
           << dSuitableAskPrice << ", " << dSuitableBidPrice
           << "]";

  const auto& order_ptr = d_->stock_object->orderInfo();
  if(order_ptr && !order_ptr->bAllMatched()) {
    // 委托回报为自动撤单，无需等待，否则为需要等待
    d_->is_auto_canceled = true;
  }

  // 重新报单
  d_->stock_object->algoQuote(dSuitableAskPrice, nAskVol, dSuitableBidPrice, nBidVol);
  d_->is_auto_quote = true;
}

bool AutoQuoteWorker::_CheckNeedPauseWhenStart()
{
  // 没有委托，不需要等待
  const auto& order_ptr = stockObject()->orderInfo();
  if(!order_ptr) {
    return false;
  }

  // 全部成交，不需要等待
  if(order_ptr->bAllMatched()) {
    return false;
  }

  d_->is_pause = true;
  emit sigAutoQuoteLog(QString::fromLocal8Bit("当前有手动报单, 策略将在手动报单撤单后自动重新报价"));
  return true;
}

int AutoQuoteWorker::getAutoQuoteInterval()
{
  QSettings settings;
  QString intervalKey = stock() + "," + "autoQuoteInterval";
  d_->auto_quote_interval = settings.value(intervalKey, 120).toInt();
  return d_->auto_quote_interval;
}

void AutoQuoteWorker::cancelOrder() {
  if(isAutoQuoteOrder()) {
    const auto& order_ptr = d_->stock_object->orderInfo();
    if(!order_ptr->bAllMatched()) {
      order_ptr->cancelOrder();
    }
  }
}

bool AutoQuoteWorker::isAutoQuoteOrder() const {
  return d_->is_auto_quote_order;
}

bool AutoQuoteWorker::isAutoQuote() const {
  return d_->is_auto_quote;
}

bool AutoQuoteWorker::isAutoCanceled() const {
  return d_->is_auto_canceled;
}

void AutoQuoteWorker::resetAutoCanceledState() {
  d_->is_auto_canceled = false;
}

IWKATS_NAMESPACE_END

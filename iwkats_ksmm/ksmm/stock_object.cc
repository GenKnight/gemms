#include "stock_object.h"

#include "core/util.hpp"
#include "ksmm/ksmm_util.h"
#include "ksmm/common_define.hpp"

#include "order/order_info.h"
#include "order/order_manager.h"
#include "match/match_info.h"
#include "match/match_manager.h"

#include "ksmm/ksmm_trader.h"

IWKATS_NAMESPACE_BEGIN

StockObject::StockObject(QObject *parent)
  : QObject(parent) {
  connect(Trader::Spi(), &KSMMTradingSpi::sigRspError, this, &StockObject::slotRspError);
  connect(MatchManager::GetPtr(), &MatchManager::sigMatchArrive, this, &StockObject::slotMatchArrive);
  connect(OrderManager::GetPtr(), &OrderManager::sigOrderArrive, this, &StockObject::slotOrderArrive);
}

StockObject::~StockObject() {
  if(order_ptr_) {
    order_ptr_->cancelOrder();
  }
}

QString StockObject::speechStock() const {
  return speech_stock_;
}

QString StockObject::stockName() const
{
  if(asset_code_) {
    return QString::fromLocal8Bit(asset_code_->szStkName);
  }
  return "";
}

void StockObject::setStock(const QString& stock)
{
  // 重置，类似构造函数
  if(stock_ != stock) {
    stock_ = stock;
    order_ptr_.reset();
    match_ptr_.reset();
    asset_code_ = Trader::Spi()->GetAssetCode(stock);

    speech_stock_ = "";

    QString::const_iterator iter(stock_.cbegin());
    for(; iter != stock_.cend(); ++iter) {
      speech_stock_.append(*iter);
      speech_stock_.append(" ");
    }
  }
}

void StockObject::algoQuote(double dAskPrice,
                            int nAskVol,
                            double dBidPrice,
                            int nBidVol)
{
  Q_ASSERT(!stock_.isEmpty());

  // 股转系统会自动撤销已有委托 ？？？？
  if(order_ptr_) {
    order_ptr_->cancelOrder();
  }

  // 构造新委托
  std::shared_ptr<ALGOQUOTE_INFO> pAlgoQuote = std::make_shared<ALGOQUOTE_INFO>();
  memset(pAlgoQuote.get(), 0, sizeof(ALGOQUOTE_INFO));
  strcpy_s(pAlgoQuote->szStkCode, stock_.toStdString().data());
  pAlgoQuote->dAskPrice = dAskPrice;
  pAlgoQuote->nAskVolume = nAskVol;
  pAlgoQuote->dBidPrice = dBidPrice;
  pAlgoQuote->nBidVolume = nBidVol;
  pAlgoQuote->nAlgoID = QUOTE_BY_HAND;
  req_id_ = KSMMUtil::GenerateReqId(type_);
  strcpy_s(pAlgoQuote->szUserDefine, req_id_.toStdString().data());

  // 设置委托回掉函数
  OrderManager::GetPtr()->SaveOrderCallback(
        req_id_, [this](const QString& order_id) {

    LOG_INFO << "股票代码: " << stock_.toStdString()
             << ", 委托类型: " << type_
             << ", 请求id: " << req_id_.toStdString()
             << ", 委托单号: " << order_id.toStdString();

  }, [this](OrderInfoPtr order_ptr) {

    // 如果当前对象没有析构 ？
    if(this) {
      /* this->order_ptr_ = order_ptr;
      emit orderInfoChanged();
      emit appendLog(order_ptr->orderDetails());*/
    }
  });

  Trader::Api()->algoQuote(pAlgoQuote);

  std::ostringstream oss;
  oss << "股票代码: " << stock_.toStdString()
      << ", 委托类型: " << type_
      << ", 叫卖价格: " << dAskPrice
      << ", 叫卖数量: " << nAskVol
      << ", 叫买价格: " << dBidPrice
      << ", 叫买数量: " << nBidVol
      << ", 请求id: " << req_id_.toStdString();
}

void StockObject::cancelOrder()
{
  if(order_ptr_) {
    order_ptr_->cancelOrder();
  }
}

void StockObject::slotOrderArrive(OrderInfoPtr order_ptr) {
  if(stock() == order_ptr->stock()) {
    this->order_ptr_ = order_ptr;
    const auto& pOrderInfo = order_ptr->GetOrderInfoPtr();

    do {

      // 委托最初回报， 叫卖数量和叫卖挂单
      if(pOrderInfo->nAskWtVolume == pOrderInfo->nAskVolume
         && pOrderInfo->nBidWtVolume == pOrderInfo->nBidVolume) {
        emit sigOrderCreated(order_ptr.get());
        break;
      }

      // 当成交数量 > 0 表示成交回报
      if(pOrderInfo->nAskMatchVol > 0 || pOrderInfo->nBidMatchVol > 0) {
        emit sigOrderMatched(order_ptr.get());
        break;
      }

      // 当撤单数量 > 0 表示撤单回报
      if(pOrderInfo->nAskCancelVol > 0 || pOrderInfo->nBidCancelVol > 0) {
        emit sigOrderCanceled(order_ptr.get());
        break;
      }

    } while(0);

    emit orderInfoChanged(order_ptr.get());
    emit appendLog(order_ptr->orderDetails());
  }
}

void StockObject::slotMatchArrive(MatchInfoPtr match_ptr)
{
  if(stock() == match_ptr->stock()) {
    match_ptr_ = match_ptr;
    emit matchInfoChanged();
    emit appendLog(match_ptr_->matchDetails());
  }
}

void StockObject::slotRspError(std::shared_ptr<LBM_ERROR_INFO> pRspError)
{
  QString stock = QString::fromLocal8Bit(pRspError->szStkCode);
  QString req_id(pRspError->szUserDefine);
  if(stock_ == stock && !req_id.isEmpty()) {
    QStringList list = req_id.split(",", QString::SkipEmptyParts);
    Q_ASSERT(list.length() == 2);
    if(type() == list.at(0).toInt()) {
      QString error = QString::fromLocal8Bit(pRspError->szErrorInfo);
      emit sigStockQuoteError(error);
    }
  }
}

IWKATS_NAMESPACE_END

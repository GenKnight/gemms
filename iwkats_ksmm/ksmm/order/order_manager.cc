#include "order_manager.h"

#include <QPair>
#include <QHash>
#include <QVector>
#include <QReadWriteLock>

#include "ksmm/ksmm_trader.h"
#include "order/order_info.h"

#include "ksmm/common_define.hpp"
#include "core/ttspeech.h"

IWKATS_NAMESPACE_BEGIN

struct OrderManager::Private {

  QReadWriteLock rwlock;

  QHash<QString, std::function<void(const QString&)>> stock_quote_callbacks_;

  // 股票代码， 委托单号， 做市回报
  QHash<QString, std::function<void(OrderInfoPtr order_ptr)>> stock_order_callbacks_;

  // 股票代码， 委托回报
  QHash<QString, QHash<QString, OrderInfoPtr>> stocks_all_orders;

  // 委托单号， 委托回报
  QHash<QString, OrderInfoPtr> all_orders;

};

//////////////////////////////////////////////////////////////////////////

OrderManager::OrderManager(QObject *parent)
  : QObject(parent)
  , d_(new Private) {

  connect(Trader::Spi(), &KSMMTradingSpi::sigOrderArrive, this, &OrderManager::slotOrderArrive);
  connect(Trader::Spi(), &KSMMTradingSpi::sigQuoteArrive, this, &OrderManager::slotQuoteArrive);
}

OrderManager* OrderManager::GetPtr() {
  static OrderManager instance;
  return &instance;
}

void OrderManager::SaveOrderCallback(const QString& req_id,
                                     const std::function<void(const QString& order_id)>& quote_callback,
                                     const std::function<void(OrderInfoPtr order_ptr)>& order_callback) {
  QWriteLocker wlock(&d_->rwlock);
  d_->stock_quote_callbacks_[req_id] = quote_callback;
  d_->stock_order_callbacks_[req_id] = order_callback;
}

OrderInfoPtr OrderManager::GetOrderPtrByOrderId(const QString& order_id) {
  QReadLocker rlock(&d_->rwlock);
  return d_->all_orders.value(order_id, nullptr);
}

void OrderManager::GetAllOrders(QList<OrderInfoPtr>& orders) {
  QReadLocker rlock(&d_->rwlock);
  orders = d_->all_orders.values();
}

void OrderManager::GetAllOrdersByStock(const QString& stock, QList<OrderInfoPtr>& orders) {
  QReadLocker rlock(&d_->rwlock);
  if(d_->stocks_all_orders.contains(stock)) {
    orders = d_->stocks_all_orders.value(stock).values();
  }
}

// 委托回报需要交易所确认才返回
void OrderManager::slotOrderArrive(std::shared_ptr<ORDER_INFO> pOrderInfo)
{
  QWriteLocker wlock(&d_->rwlock);

  OrderInfoPtr order_ptr = std::make_shared<OrderInfo>(pOrderInfo);

  auto& order_callback = d_->stock_order_callbacks_.value(order_ptr->orderId(), nullptr);
  if(order_callback) {
    order_callback(order_ptr);
  }

  // 如果全部成交，则移除委托回掉
  if(pOrderInfo->bAllMatched) {
    d_->stock_order_callbacks_.remove(order_ptr->orderId());
  }

  d_->all_orders.insert(order_ptr->orderId(), order_ptr);
  d_->stocks_all_orders[pOrderInfo->szStkCode].insert(order_ptr->orderId(), order_ptr);

  emit sigOrderArrive(order_ptr);
}

// 报价回报先回
void OrderManager::slotQuoteArrive(std::shared_ptr<QUOTE_BACK_INFO> pBackQuote)
{
  QWriteLocker rlock(&d_->rwlock);

  // 返回order_id
  QString req_id = pBackQuote->szUserDefine;
  const auto& quote_callback = d_->stock_quote_callbacks_.value(req_id, nullptr);
  if(quote_callback) {
    quote_callback(pBackQuote->szOrderID);
  }

  const auto& order_callback = d_->stock_order_callbacks_.value(req_id, nullptr);
  if(order_callback) {
    d_->stock_order_callbacks_.insert(pBackQuote->szOrderID, order_callback);
  }

  d_->stock_order_callbacks_.remove(req_id);
  d_->stock_quote_callbacks_.remove(req_id);
}


IWKATS_NAMESPACE_END

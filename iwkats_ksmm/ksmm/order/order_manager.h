#pragma once

#include "ksmm/common_define.hpp"
#include "core/iwkats_core.h"
#include "core/util.hpp"

#include <QList>
#include <QObject>
#include <functional>

IWKATS_NAMESPACE_BEGIN
// 委托回报
class IWKATS_CORE_EXPORT OrderManager : public QObject {
  Q_OBJECT
  OrderManager(QObject* parent = nullptr);
public:
  static OrderManager* GetPtr();

  void SaveOrderCallback(const QString& req_id,
                         const std::function<void(const QString& order_id)>& quote_callback,
                         const std::function<void(OrderInfoPtr order_ptr)>& order_callback);

  OrderInfoPtr GetOrderPtrByOrderId(const QString& order_id);

  void GetAllOrders(QList<OrderInfoPtr>& orders);

  void GetAllOrdersByStock(const QString& stock, QList<OrderInfoPtr>& orders);

public slots:
  void slotOrderArrive(std::shared_ptr<ORDER_INFO> pOrderInfo);
  void slotQuoteArrive(std::shared_ptr<QUOTE_BACK_INFO> pBackQuote);

signals:
  void sigOrderArrive(OrderInfoPtr order_ptr);
  void sigHistoryOrderArrive(const QString& stock, const QString& order_id);

private:
  struct Private;
  std::shared_ptr<Private> d_;
};

IWKATS_NAMESPACE_END

#pragma once

#include "core/iwkats_core.h"

#include <QObject>

#include "ksmm/common_define.hpp"

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT OrderInfo : public QObject {
  Q_OBJECT
  Q_PROPERTY(double dAskPrice READ dAskPrice NOTIFY dAskPriceChanged)
  Q_PROPERTY(int nAskVol READ nAskVol)
  Q_PROPERTY(double dBidPrice READ dBidPrice NOTIFY dBidPriceChanged)
  Q_PROPERTY(int nBidVol READ nBidVol)
  Q_PROPERTY(QString askOrderStatus READ askOrderStatus)
  Q_PROPERTY(QString bidOrderStatus READ bidOrderStatus)
  Q_PROPERTY(int nAskWtVol READ nAskWtVol)
  Q_PROPERTY(int nBidWtVol READ nBidWtVol)

  Q_PROPERTY(QString infoBarMsg READ infoBarMsg)
  Q_PROPERTY(QString orderId READ orderId)
  Q_PROPERTY(bool bAllMatched READ bAllMatched NOTIFY bAllMatchedChanged)
public:
  explicit OrderInfo(std::shared_ptr<ORDER_INFO> order_ptr);

  // getter
  QString stock() const { return order_ptr_->szStkCode; }
  double dAskPrice() const { return order_ptr_->dAskPrice; }
  int nAskVol() const { return order_ptr_->nAskVolume; }
  double dBidPrice() const { return order_ptr_->dBidPrice; } 
  int nBidVol() const { return order_ptr_->nBidVolume; }
  QString askOrderStatus() const { return GetOrderStatusMean(order_ptr_->chAskOrderStatus); }
  QString bidOrderStatus() const { return GetOrderStatusMean(order_ptr_->chBidOrderStatus); }

  int nAskWtVol() const { return order_ptr_->nAskWtVolume; }
  int nBidWtVol() const { return order_ptr_->nBidWtVolume; }

  QString orderDetails() const { return order_details_; }
  QString infoBarMsg() const { return info_msg_; }
  QString orderId() const { return order_ptr_->szOrderID; }
  bool bAllMatched() const { return order_ptr_->bAllMatched; }


  std::shared_ptr<ORDER_INFO> GetOrderInfoPtr() const { return order_ptr_; }

  // method
  void cancelOrder();

signals:
  void dAskPriceChanged();
  void dBidPriceChanged();
  void bAllMatchedChanged();

private:
  QString order_details_{""};
  QString info_msg_{""};
  std::shared_ptr<ORDER_INFO> order_ptr_{nullptr};
};

IWKATS_NAMESPACE_END

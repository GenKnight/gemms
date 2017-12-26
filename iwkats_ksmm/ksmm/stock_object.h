#pragma once

#include "core/iwkats_core.h"

#include <QObject>

#include "ksmm/common_define.hpp"

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT StockObject : public QObject {
  Q_OBJECT
  Q_ENUMS(StockType)
  Q_PROPERTY(QString stock READ stock WRITE setStock)
  Q_PROPERTY(StockType type READ type WRITE setType)
  Q_PROPERTY(QString stockName READ stockName)
  Q_PROPERTY(OrderInfo* orderInfo READ orderInfo NOTIFY orderInfoChanged)
  Q_PROPERTY(MatchInfo* matchInfo READ matchInfo NOTIFY matchInfoChanged)
public:
  enum StockType {
    QUOTE_BY_HAND = 1,        // 默认
    QUOTE_BY_AUTO_QUOTE,     // 自动重复
  };

  StockObject(QObject* parent = nullptr);
  ~StockObject();

  // getter
  QString stock() const { return stock_; }
  QString speechStock() const;
  StockType  type() const { return type_; }
  QString stockName() const;

  OrderInfo* orderInfo() const { return order_ptr_.get(); }
  MatchInfo* matchInfo() const { return match_ptr_.get(); }
  ASSET_CODE* assetCode() const { return asset_code_; }

  // setter
  void setStock(const QString& stock);
  void setType(StockType type) { type_ = type; }

  // 委托下单
  Q_INVOKABLE void algoQuote(double dAskPrice,
                             int nAskVol,
                             double dBidPrice,
                             int nBidVol);

  // 撤单
  Q_INVOKABLE void cancelOrder();

public slots:
  // 委托返回
  void slotOrderArrive(OrderInfoPtr order_ptr);

  // 成交返回
  void slotMatchArrive(MatchInfoPtr match_ptr);

  void slotRspError(std::shared_ptr<LBM_ERROR_INFO> pRspError);

signals:
  void sigStockQuoteError(const QString& error);
  void orderInfoChanged(OrderInfo* order_info);
  void matchInfoChanged();
  void appendLog(const QString& msg);

  // 新委托
  void sigOrderCreated(OrderInfo* order_info);

  // 撤单
  void sigOrderCanceled(OrderInfo* order_info);

  // 成交委托
  void sigOrderMatched(OrderInfo* order_info);

private:
  QString stock_{""};
  QString speech_stock_{""};
  QString req_id_{""};
  StockType type_{QUOTE_BY_HAND};
  OrderInfoPtr order_ptr_{nullptr};
  MatchInfoPtr match_ptr_{nullptr};
  ASSET_CODE* asset_code_{nullptr};
};

IWKATS_NAMESPACE_END

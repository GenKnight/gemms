#pragma once

#include "core/iwkats_core.h"
#include "ksmm/common_define.hpp"

#include <QObject>

IWKATS_NAMESPACE_BEGIN

class AutoQuoteObject;
class AutoQuoteWorker : public QObject {
  Q_OBJECT
  Q_PROPERTY(StockObject* stockObject READ stockObject)
public:
  AutoQuoteWorker(const QString& stock,
                  QObject* parent = nullptr);

  QString stock() const;
  QString speechStock() const;

  QString speechStockName() const;

  bool isRunning() const;
  bool isWait() const;

  bool isPause() const;

  StockObject* stockObject() const;

protected slots:
  friend class AutoQuoteObject;
  // 开始自动化做市
  void startAutoQuote();
  // 停止自动化做市
  void stopAutoQuote();

public slots:
  // 委托创建，成交，撤单变化
  void slotOrderCreated();
  void slotOrderMatched();
  void slotOrderCanceled();

  // 做市行情返回
  void slotZsTickArrive(std::shared_ptr<STK_NQ_ZSXX> pZSHQInfo);
  // 等待时间结束
  void autoQuoteWaitTimeout();
  // 股票日志
  void slotStockAppendLog(const QString& msg);

signals:
  void sigAutoQuoteLog(QString log);
  void sigStockQuoteError(QString error);
  void sigOrderInfoChanged(OrderInfo* orderInfo);

  void sigAutoQuoteStarted();
  void sigAutoQuoteStoped();

private:
  bool startWaitTimer();
  void _AutoQuote(double dAskPrice1, double dBidPrice1, int nAskVol = 1000, int nBidVol = 1000);

  bool _CheckNeedPauseWhenStart();
  int getAutoQuoteInterval();

  void cancelOrder();

  bool isAutoQuoteOrder() const;
  bool isAutoQuote() const;
  bool isAutoCanceled() const;
  void resetAutoCanceledState();

private:
  struct Private;
  std::shared_ptr<Private> d_;
};


IWKATS_NAMESPACE_END

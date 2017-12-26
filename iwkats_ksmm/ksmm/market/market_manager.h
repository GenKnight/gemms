#pragma once

#include "core/iwkats_core.h"
#include "ksmm/common_define.hpp"

#include <QObject>

IWKATS_NAMESPACE_BEGIN

/**
 * @brief 行情订阅管理类
 */
class IWKATS_CORE_EXPORT MarketManager : public QObject {
  Q_OBJECT
  MarketManager(QObject* parent = nullptr);
public:
  static MarketManager* GetPtr();

  /// 订阅行情
  int subscribe(const QString& stock);

  // 取消订阅
  int unsubscribe(const QString& stock);

  // 获取当前普通行情
  std::shared_ptr<STK_NQ_TICK> GetStockNQTick(const QString& stock);

  // 获取当前做市行情
  std::shared_ptr<STK_NQ_ZSXX> GetStockZSTick(const QString& stock);


public slots:
  void slotNqTickArrive(std::shared_ptr<STK_NQ_TICK> pNQHQInfo);
  void slotZsTickArrive(std::shared_ptr<STK_NQ_ZSXX> pZSHQInfo);

signals:
  // 普通行情返回
  void sigNqTickArrive(std::shared_ptr<STK_NQ_TICK> pNQHQInfo);
  // 做市行情返回
  void sigZsTickArrive(std::shared_ptr<STK_NQ_ZSXX> pZSHQInfo);

private:
  void emitCurrentCacheTick(const QString& stock);

private:
  struct Private;
  std::shared_ptr<Private> d_;
};

IWKATS_NAMESPACE_END

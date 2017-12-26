#pragma once

#include "core/iwkats_core.h"

#include "ksmm_trading_spi.h"

#include <QObject>

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT KSMMTradingApi : public QObject {
  Q_OBJECT
public:
  KSMMTradingApi(QObject* parent = nullptr);
  virtual ~KSMMTradingApi();

  KSMMTradingSpi* Spi() const;

  /// 订阅行情
  int subscribe(const QString& code);

  // 取消订阅
  int unsubscribe(const QString& code);

signals:
  // 请求登陆
  void requestLogin(QString user_name, QString password);

  // 做市报价
  bool quote(const QString& code,
             double dAskPrice,
             int nAskVol,
             double dBidPrice,
             int nBidVol);

  bool algoQuote(std::shared_ptr<ALGOQUOTE_INFO> pAlgoQuoteInfo);

  // 做市撤单申报
  /// @param order_id 委托合同序号
  /// @param order_type '0':做市; '1':自营
  int cancelOrder(const QString& order_id, QString order_type = "0");
  // 取操作员资金
  int getFunds(int asset_id);
  // 取操作员的持仓
  int getShare(const QString& stk_code, bool zero = false);
  /***
  * 查询做市交易委托单
  * search_type '0'-所有,'1'-最新,'2'-双边
  */
  int listQuote(const QString& stk_code,
                int beg_time,
                int end_time,
                int beg_date,
                int end_date,
                char search_type = '0');
  // 查询成交
  int listQuoteMatch(const QString& stk_code,
                     int beg_time,
                     int end_time,
                     int beg_date,
                     int end_date,
                     char search_type = '0');
  // 查询资产
  int queryAssetFund(int nAssetID = 0);
  /**
  * 取操作员的持仓
  * @param[in] nAssetID 资产单元ID(0-表示当前用户的所有资产单元)
  * @param[in] bZero 是否返回持仓为0的记录
  * @return return 0: 成功，其它：错误码
  */
  int queryAssetShare(const QString& stock, int nAssetID = 0, BOOL bZero = true);

private:
  struct Private;
  std::shared_ptr<Private> d_;
};

IWKATS_NAMESPACE_END

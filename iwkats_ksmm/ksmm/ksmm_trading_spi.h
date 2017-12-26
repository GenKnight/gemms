#pragma once

#include <QObject>
#include <memory>
#include <qt_windows.h>

#include "core/iwkats_core.h"
#include "core/lock.h"
#include "core/util.hpp"

#include "KSMMApi/Define.h"
#include "KSMMApi/LBM_Plugin.h"

#pragma comment(lib, "KCBPCli.lib")
#pragma comment(lib, "KSMMApi.lib")
#pragma comment(lib, "encrypt.lib")

IWKATS_NAMESPACE_BEGIN

class KSMMTradingApi;
class MarketManager;
class IWKATS_CORE_EXPORT KSMMTradingSpi : public QObject {
  Q_OBJECT
public:
  KSMMTradingSpi(QObject* parent = nullptr);
  ~KSMMTradingSpi();

private:
  friend class KSMMTrading;
  // 是否初始化完毕
  bool IsReadyTrade() const;
  // 读取配置文件
  void ReadConfig();
  // 设置回调函数
  void SetCallback();
  // 同步系统时间
  void SyncSrvTime();
  void Int64ToSystemTime(long long llDateTime, SYSTEMTIME& t_out);

public:
  // 获取可操作的股票列表
  void GetAssetCodes(std::vector<ASSET_CODE*>& codes);
  QString getLastError() const;
  ASSET_CODE* GetAssetCode(const QString& code);

protected slots:
  friend class KSMMTradingApi;
  friend class MarketManager;
  // 登陆服务器
  bool Login(QString user_name, QString password);

  /// 订阅行情
  int subscribe(const QString& code);

  // 取消订阅
  int unsubscribe(const QString& code);

  // 做市报价
  int quote_(std::shared_ptr<QUOTE_INFO>& info);

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

signals:
  // 登陆状态
  void loginStatusChanged(QString msg);
  // 登陆结果
  void loginResult(bool isOk);
  // 普通行情返回
  void sigNqTickArrive(std::shared_ptr<STK_NQ_TICK> pNQHQInfo);
  // 做市行情返回
  void sigZsTickArrive(std::shared_ptr<STK_NQ_ZSXX> pZSHQInfo);
  // 委托回调
  void sigOrderArrive(std::shared_ptr<ORDER_INFO> pOrderInfo);
  // 成交回调
  void sigMatchArrive(std::shared_ptr<MATCH_INFO> pMatchInfo);
  // 双边报价回掉
  void sigQuoteArrive(std::shared_ptr<QUOTE_BACK_INFO> pBackQuote);
  // 查询资金
  void sigQueryFundArrive(SharedVecPtr<ASSET_FUND> pAssetFund);
  // 查询仓位
  void sigQueryShareArrive(SharedVecPtr<ASSET_SHARE> pAssetShare);
  // 双标报价
  void sigQuoteListArrive(SharedVecPtr<QUOTE_ORDER_INFO> pQuoteOrderInfo);
  // 查询成交
  void sigQuoteMatchArrive(SharedVecPtr<QUOTE_MATCH_INFO> pQuoteMatchInfo);
  // LBM错误
  void sigRspError(std::shared_ptr<LBM_ERROR_INFO> pRspError);
  // HQ 错误
  void sigHqError(QString error);
  // 主线程错误
  void sigMainThreadStop(QString error);

  // 相关回调函数
public:
  // 普通行情回调
  static void OnNQTick(CALLBACK_PARAM * pParam, STK_NQ_TICK * pTick);
  // 做市行情回调
  static void OnZSTick(CALLBACK_PARAM * pParam, STK_NQ_ZSXX * pTick);
  // 委托回报回调
  static void OnOrder(CALLBACK_PARAM * pParam, ORDER_INFO * pOrder);
  // 成交回报回调
  static void OnTrade(CALLBACK_PARAM * pParam, MATCH_INFO * pMatch);
  // 行情获取失败
  static void OnHQError(CALLBACK_PARAM * pParam, char * pError);
  // 策略主线程停止回调
  static void OnMainTreadStop(CALLBACK_PARAM * pParam, char * pError);
  // LBM错误信息回调
  static void OnGetError(CALLBACK_PARAM * pParam, LBM_ERROR_INFO * pszErrorInfo);
  // 双边报价回调
  static void OnQuote(CALLBACK_PARAM * pParam, QUOTE_BACK_INFO * pQuoteBackInfo);
  // 委托列表信息回调
  static void OnQuoteList(CALLBACK_PARAM * pParam, QUOTE_ORDER_INFO * pQuote, int nItem);
  // 成交列表回调
  static void OnQuoteMatchList(CALLBACK_PARAM * pParam, QUOTE_MATCH_INFO * pMatchInfo, int nItem);
  // 资金回调
  static void OnFund(CALLBACK_PARAM * pParam, ASSET_FUND * pAssetFund, int nItem);
  // 操作员持仓回调
  static void OnShare(CALLBACK_PARAM * pParam, ASSET_SHARE * pAssetShare, int nItem);

private:
  struct Private;
  std::shared_ptr<Private> d_;

  LBM_Plugin* lbm_plugin_{nullptr};
};

IWKATS_NAMESPACE_END

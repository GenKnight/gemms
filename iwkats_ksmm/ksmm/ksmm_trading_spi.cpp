#include "ksmm_trading_spi.h"

#include <QHash>

#include "core/logging.h"

#include "common_define.hpp"
#include "xsdk_datetime.h"

IWKATS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////

struct KSMMTradingSpi::Private {
  ST_tagKCBP_CONFIG kcbp_config;	          // KCBP 连接信息
  std::string user_name;
  std::string password;
  bool is_back_all_hq{false};

  ASSET_CODE * p_code_info{nullptr};
  QHash<QString, ASSET_CODE*> asset_codes;

  int code_nums{0};                              // 可操作股票数量
  int hq_error_num{0};                           // 行情获取失败次数
  int tick_time{0};                              // 行情间隔
  int order_interval{0};                         // 委托确认间隔
  int ready_trade{RTMSG_KO};                // 是否初始化
  QReadWriteLock rwlock_;

  QString last_error;
  std::set<QString> subscribes;

  int m_nHqErrorNum{0};								///< 行情获取失败次数
  int m_nTickTime{4000};								///< 行情间隔
  int m_nOrderTime{1000};								///< 委托确认间隔
  int m_nConnectTime{10000};								///< 连接轮询间隔
};

//////////////////////////////////////////////////////////////////////////

KSMMTradingSpi::KSMMTradingSpi(QObject* parent)
  : QObject(parent)
  , d_(new Private) {

}

KSMMTradingSpi::~KSMMTradingSpi() {
  if(lbm_plugin_) {
    lbm_plugin_->Close();
    delete lbm_plugin_;
    lbm_plugin_ = nullptr;
  }
}

bool KSMMTradingSpi::Login(QString user_name, QString password) {
  if(!lbm_plugin_) {
    lbm_plugin_ = new LBM_Plugin;
  }

  // 读取配置文件
  ReadConfig();

  d_->user_name = user_name.toStdString();
  d_->password = password.toStdString();

  LOG_INFO << "正在准备连接到KCBP...";

  emit loginStatusChanged(QString::fromLocal8Bit("准备连接到KCBP..."));

  ST_tagInitInfo init_info{0};
  strcpy_s(init_info.szUserName, d_->user_name.data());
  strcpy_s(init_info.szPassword, d_->password.data());
  init_info.bBackAllHQ = false;

  do {
    if(RTMSG_OK != lbm_plugin_->InitPlugin(&init_info, d_->kcbp_config, 2)) {
      LOG_ERROR << "KCBP 连接失败！请检查相关配置";
      emit loginStatusChanged(QString::fromLocal8Bit("KCBP 连接失败！请检查相关配置"));
      break;
    }

    emit loginStatusChanged(QString::fromLocal8Bit("连接KCBP成功"));
    emit loginStatusChanged(QString::fromLocal8Bit("正在同步系统时间..."));

    // 需同步本地时间和kcbp时间差
    SyncSrvTime();

    // 初始化算法
    if(RTMSG_OK != lbm_plugin_->InitAlgo(&d_->p_code_info, d_->code_nums)) {
      emit loginStatusChanged(QString::fromLocal8Bit("KCBP 初始化算法失败"));
      LOG_ERROR << ("KCBP 初始化算法获取可操作股票列表失败");
      break;
    }

    for(auto i = 0; i < d_->code_nums; ++i) {
      d_->asset_codes[d_->p_code_info[i].szStkCode] = &d_->p_code_info[i];
    }

    // 设置间隔时间配置
    if(!lbm_plugin_->SetConfigTime(d_->m_nTickTime, d_->m_nOrderTime, d_->m_nConnectTime)) {
      emit loginStatusChanged(QString::fromLocal8Bit("轮询间隔时间必须大于0"));
      LOG_ERROR << ("轮询间隔时间必须大于0");
      break;
    }
    // 设置行情出错退出线程次数
    if(!lbm_plugin_->SetHQErrorNum(d_->hq_error_num)) {
      emit loginStatusChanged(QString::fromLocal8Bit("行情出错退出次数必须大于0"));
      LOG_ERROR << ("行情出错退出次数必须大于0");
      break;
    }

    // 设置异步回调函数
    SetCallback();
    LOG_INFO << ("KCBP 连接成功 Success");
    emit loginStatusChanged(QString::fromLocal8Bit("登陆系统成功"));
    d_->ready_trade = RTMSG_OK;
  } while(0);

  emit loginResult(0 == d_->ready_trade);
  return true;
}

bool KSMMTradingSpi::IsReadyTrade() const {
  return RTMSG_OK == d_->ready_trade;
}

void KSMMTradingSpi::ReadConfig() {
  // 获取ini文件路径
  char szFilePath[MAX_PATH + 1] = {0};
  GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
  std::string strFilePath(szFilePath);
  auto pos = strFilePath.find_last_of("\\");
  strFilePath.erase(pos);
  strFilePath = strFilePath + "\\config.ini";
  LOG_INFO << "正在读取 " << strFilePath << " 配置文件...";

  emit loginStatusChanged(QString::fromLocal8Bit("正在读取配置文件..."));

  // 读取KCBP连接配置
  GetPrivateProfileStringA("kcbp", "username", "KCXP00", d_->kcbp_config.szUserName, sizeof(d_->kcbp_config.szUserName), strFilePath.data());
  GetPrivateProfileStringA("kcbp", "password", "888888", d_->kcbp_config.szPassword, sizeof(d_->kcbp_config.szPassword), strFilePath.data());
  GetPrivateProfileStringA("kcbp", "address", "127.0.0.1", d_->kcbp_config.szIPAddress, sizeof(d_->kcbp_config.szIPAddress), strFilePath.data());
  GetPrivateProfileStringA("kcbp", "server", "KCBP1", d_->kcbp_config.szServer, sizeof(d_->kcbp_config.szServer), strFilePath.data());
  GetPrivateProfileStringA("kcbp", "kcxpreq", "req_market", d_->kcbp_config.szReq, sizeof(d_->kcbp_config.szReq), strFilePath.data());
  GetPrivateProfileStringA("kcbp", "kcxpans", "ans_market", d_->kcbp_config.szAns, sizeof(d_->kcbp_config.szAns), strFilePath.data());
  d_->kcbp_config.nPort = GetPrivateProfileIntA("kcbp", "port", 21000, strFilePath.data());

  // 读取行情配置
  d_->hq_error_num = GetPrivateProfileIntA("hq", "HQ_ERRORNUM", 3, strFilePath.data());

  // 读取系统时间间隔
  d_->tick_time = GetPrivateProfileIntA("system", "TICKTIME", 4000, strFilePath.data());
  d_->order_interval = GetPrivateProfileIntA("system", "ORDERTIME", 1000, strFilePath.data());

  // 登录账号
  ST_tagInitInfo init_info{0};
  memset(&init_info, 0, sizeof(init_info));
  GetPrivateProfileStringA("KSMM", "username", "8888", init_info.szUserName, sizeof(init_info.szUserName), strFilePath.data());
  GetPrivateProfileStringA("KSMM", "passwd", "888888", init_info.szPassword, sizeof(init_info.szPassword), strFilePath.data());
  init_info.bBackAllHQ = GetPrivateProfileIntA("KSMM", "backallhq", 1, strFilePath.data());

  d_->user_name = init_info.szUserName;
  d_->password = init_info.szPassword;
  d_->is_back_all_hq = init_info.bBackAllHQ;

  LOG_INFO << "读取配置文件完毕";

  emit loginStatusChanged(QString::fromLocal8Bit("读取配置文件成功"));
}

void KSMMTradingSpi::SetCallback() {
  // 回调函数在获取后，需立即赋值后，返回主线程。

  // 设置行情回调函数
  lbm_plugin_->SetCallBackHQFunction((HWND)this, OnNQTick, OnZSTick);

  // 设置委托回调函数
  lbm_plugin_->SetCallBackWTFunction((HWND)this, OnOrder, OnTrade);

  // 设置主线程异常回调函数
  lbm_plugin_->SetCallBackMainThread((HWND)this, OnHQError, OnMainTreadStop);

  // 设置通用错误回调函数
  lbm_plugin_->SetCallBackLBMError((HWND)this, OnGetError);

  // 设置双边报价回调函数
  lbm_plugin_->SetCallBackQuote((HWND)this, OnQuote);

  // 委托列表信息回调
  lbm_plugin_->SetCallBackQuoteList((HWND)this, OnQuoteList);

  // 成交列表回调
  lbm_plugin_->SetCallBackQuoteMatchList((HWND)this, OnQuoteMatchList);

  // 资金回调
  lbm_plugin_->SetCallBackFund((HWND)this, OnFund);

  // 操作员持仓回调
  lbm_plugin_->SetCallBackShare((HWND)this, OnShare);
}

void KSMMTradingSpi::SyncSrvTime() {
  int nDate, nTime = 0;
  __int64 llSvrDateTime = 0, llCurDateTime = 0, llDiffentTime = 0;
  SYSTEMTIME t1 = {0};
  SYSTEMTIME t2 = {0};
  string strErr = "";
  char szBuf[256] = {0};
  lbm_plugin_->GetSvrTime(nDate, nTime);
  GetLocalTime(&t1);
  llCurDateTime = (t1.wYear * 10000 + t1.wMonth * 100 + t1.wDay);
  llCurDateTime *= 100000000;
  llCurDateTime += t1.wHour * 1000000 + t1.wMinute * 10000 + t1.wSecond * 100 + t1.wMilliseconds / 10;
  llSvrDateTime = (__int64)nDate * 100000000 + (__int64)nTime;
  if(abs(llCurDateTime - llSvrDateTime) > 10) {
    Int64ToSystemTime(llSvrDateTime, t2);
    llDiffentTime = DatetimeDiff(t1, t2, DT_MILLISECONDS);
    lbm_plugin_->SetSvrDiffrent(llDiffentTime);
  }
}

void KSMMTradingSpi::Int64ToSystemTime(long long llDateTime, SYSTEMTIME& t_out) {
  t_out.wDayOfWeek = 0;
  t_out.wYear = (WORD)(llDateTime / 1000000000000);
  llDateTime %= 1000000000000;
  t_out.wMonth = (WORD)(llDateTime / 10000000000);
  llDateTime %= 10000000000;
  t_out.wDay = (WORD)(llDateTime / 100000000);
  llDateTime %= 100000000;
  t_out.wHour = (WORD)(llDateTime / 1000000);
  llDateTime %= 1000000;
  t_out.wMinute = (WORD)(llDateTime / 10000);
  llDateTime %= 10000;
  t_out.wSecond = (WORD)(llDateTime / 100);
  llDateTime %= 100;
  t_out.wMilliseconds = (WORD)(llDateTime * 10);
}

void KSMMTradingSpi::GetAssetCodes(std::vector<ASSET_CODE*>& codes) {
  iwkats::RLockGuard lock(d_->rwlock_);
  for(auto i = 0; i < d_->code_nums; ++i) {
    codes.push_back(&d_->p_code_info[i]);
  }
}

QString KSMMTradingSpi::getLastError() const {
  return d_->last_error;
}

ASSET_CODE* KSMMTradingSpi::GetAssetCode(const QString& code) {
  if(d_->asset_codes.contains(code)) {
    return d_->asset_codes.value(code);
  }
  return nullptr;
}

/// 订阅行情
int KSMMTradingSpi::subscribe(const QString& code) {
  LOG_DEBUG << "开始订阅: " << code.toStdString();
  int ret = -1;
  do {
    /// 判断订阅代码是否为空
    if(code.isEmpty()) {
      ret = kInputParamInvalid;
      d_->last_error = GetErrorMsgByErrorType(ret);
      LOG_ERROR << d_->last_error.toStdString();
      break;
    }

    /// 判断是否重复订阅
    {
      RLockGuard lock(d_->rwlock_);
      if(d_->subscribes.count(code)) {
        ret = kSuccess;
        break;
      }
    }

    /// 订阅
    ret = lbm_plugin_->SubScribe(const_cast<char*>(code.toStdString().data()));
    if(RTMSG_OK != ret) {
      ret = kCallLBMPluginFailed;
      d_->last_error = GetErrorMsgByErrorType(ret);
      LOG_INFO << "订阅失败: " << d_->last_error.toStdString();
      break;
    }

    /// 保存订阅
    {
      WLockGuard lock(d_->rwlock_);
      d_->subscribes.insert(code);
    }
    LOG_DEBUG << "订阅成功: " << code.toStdString();
  } while(0);
  return ret;
}

/// 退订行情
int KSMMTradingSpi::unsubscribe(const QString &code) {
  LOG_DEBUG << "开始退订: " << code.toStdString();
  int ret = -1;
  do {
    /// 判断代码是否为空
    if(code.isEmpty()) {
      ret = kInputParamInvalid;
      LOG_ERROR << "退订失败，code不能为空";
      break;
    }
    /// 判断退订股票是否存在
    {
      RLockGuard lock(d_->rwlock_);
      if(!d_->subscribes.count(code)) {
        ret = kStockNotExist;
        LOG_INFO << "股票code[" << code.toStdString() << "]不存在，忽略退订";
        break;
      }
    }

    /// 取消订阅
    ret = lbm_plugin_->UnSubScribe(const_cast<char*>(code.toStdString().data()));
    if(RTMSG_OK != ret) {
      ret = kCallLBMPluginFailed;
      d_->last_error = GetErrorMsgByErrorType(ret);
      LOG_INFO << "退订失败：" << d_->last_error.toStdString();
      break;
    }

    {
      WLockGuard lock(d_->rwlock_);
      d_->subscribes.erase(code);
    }
    LOG_DEBUG << "退订成功: " << code.toStdString();

  } while(0);
  return ret;
}

/// 做市报价
int KSMMTradingSpi::quote_(std::shared_ptr<QUOTE_INFO>& info) {
  int ret = -1;
  do {
    /// 检查数量和价格是否合法
    if(info->dAskPrice <= 0
       || info->dBidPrice <= 0
       || info->nAskVolume % 1000 != 0
       || info->nBidVolume % 1000 != 0) {
      ret = kInputParamInvalid;
      LOG_ERROR << "请检查数量和价格。（价格必须大于0，数量为1000的整数倍)";
      break;
    }

    /// 报价
    ret = lbm_plugin_->Quote(info.get());
    if(RTMSG_OK != ret) {
      ret = kCallLBMPluginFailed;
      d_->last_error = GetErrorMsgByErrorType(ret);
      LOG_INFO << "做市报价调用失败:" << d_->last_error.toStdString();
      break;
    }

  } while(0);
  return ret;
}

bool KSMMTradingSpi::quote(const QString& code,
                           double dAskPrice,
                           int nAskVol,
                           double dBidPrice,
                           int nBidVol) {
  std::shared_ptr<QUOTE_INFO> pQuote = std::make_shared<QUOTE_INFO>();
  memset(pQuote.get(), 0, sizeof(QUOTE_INFO));
  strcpy_s(pQuote->szStkCode, code.toStdString().data());
  pQuote->dAskPrice = dAskPrice;
  pQuote->nAskVolume = nAskVol;
  pQuote->dBidPrice = dBidPrice;
  pQuote->nBidVolume = nBidVol;
  auto ret = quote_(pQuote);
  return RTMSG_OK == ret;
}

bool KSMMTradingSpi::algoQuote(std::shared_ptr<ALGOQUOTE_INFO> pAlgoQuoteInfo)
{
  int ret = -1;
  do {
    /// 检查数量和价格是否合法
    if(pAlgoQuoteInfo->dAskPrice <= 0
       || pAlgoQuoteInfo->dBidPrice <= 0
       || pAlgoQuoteInfo->nAskVolume % 1000 != 0
       || pAlgoQuoteInfo->nBidVolume % 1000 != 0) {
      ret = kInputParamInvalid;
      LOG_ERROR << "请检查数量和价格。（价格必须大于0，数量为1000的整数倍)";
      break;
    }

    /// 报价
    ret = lbm_plugin_->AlgoQuote(pAlgoQuoteInfo.get());
    if(RTMSG_OK != ret) {
      ret = kCallLBMPluginFailed;
      d_->last_error = GetErrorMsgByErrorType(ret);
      LOG_INFO << "做市报价调用失败:" << d_->last_error.toStdString();
      break;
    }

  } while(0);

  return 0 == ret;
}

/// 做市撤单申报
int KSMMTradingSpi::cancelOrder(const QString &order_id, QString order_type) {
  int ret = -1;
  do {
    if(order_id.isEmpty() || order_type.isEmpty()) {
      ret = kInputParamInvalid;
      LOG_ERROR << "传入参数有误. 委托合同序号:" << order_id.isEmpty() << order_type.isEmpty();
      break;
    }

    ret = lbm_plugin_->CancelOrder(const_cast<char*>(order_id.toStdString().data()),
                                   *(order_type.toStdString().data()));
    if(RTMSG_OK != ret) {
      ret = lbm_plugin_->CancelOldOrder(const_cast<char*>(order_id.toStdString().data()),
                                        *(order_type.toStdString().data()));
      if(RTMSG_OK != ret) {
        ret = kCallLBMPluginFailed;
        d_->last_error = GetErrorMsgByErrorType(ret);
        LOG_ERROR << "做市撤单调用失败：" << d_->last_error.toStdString();
        break;
      }
    }
  } while(0);

  return ret;
}

// 取操作员资金
int KSMMTradingSpi::getFunds(int asset_id) {
  auto ret = lbm_plugin_->GetFunds(asset_id);
  if(RTMSG_OK != ret) {
    d_->last_error = "调用取操作员资金失败";
    LOG_ERROR << d_->last_error.toStdString();
    return ret;
  }
  return RTMSG_OK;
}

// 取操作员仓位
int KSMMTradingSpi::getShare(const QString& stk_code, bool zero) {
  int asset_id;
  ASSET_CODE* asset_code = GetAssetCode(stk_code);
  if(!asset_code) {
    d_->last_error = stk_code + core_util::toUtf8(" 不是可操作股票");
    return RTMSG_KO;
  }

  auto ret = lbm_plugin_->GetShare(asset_code->nAssetID, zero, const_cast<char*>(stk_code.toStdString().data()));
  if(RTMSG_OK != ret) {
    d_->last_error = "取操作员仓位失败";
    LOG_ERROR << d_->last_error.toStdString();
    return ret;
  }
  return RTMSG_OK;
}

// 查询做市交易委托单
int KSMMTradingSpi::listQuote(const QString& stk_code,
                              int beg_time,
                              int end_time,
                              int beg_date,
                              int end_date,
                              char search_type) {
  ORDER_SEARCH pOrder;
  memset(pOrder.szStkCode, 0, sizeof(pOrder.szStkCode));
  strcpy_s(pOrder.szStkCode, stk_code.toStdString().data());
  pOrder.nBegTime = beg_time;
  pOrder.nEndTime = end_time;
  pOrder.nBegDate = beg_date;
  pOrder.nEndDate = end_date;
  pOrder.chSearchType[0] = search_type;

  auto ret = lbm_plugin_->List_Quote(pOrder);
  if(RTMSG_OK != ret) {
    d_->last_error = "查询做事交易委托单失败";
    LOG_ERROR << d_->last_error.toStdString();
    return ret;
  }
  return RTMSG_OK;
}

// 查询成交
int KSMMTradingSpi::listQuoteMatch(const QString& stk_code,
                                   int beg_time,
                                   int end_time,
                                   int beg_date,
                                   int end_date,
                                   char search_type) {
  ORDER_SEARCH match_search;
  memset(match_search.szStkCode, 0, sizeof(match_search.szStkCode));
  strcpy_s(match_search.szStkCode, stk_code.toStdString().data());

  match_search.nBegTime = beg_time;
  match_search.nEndTime = end_time;
  match_search.nBegDate = beg_date;
  match_search.nEndDate = end_date;
  match_search.chSearchType[0] = search_type;

  auto ret = lbm_plugin_->List_QuoteMatch(match_search);
  if(RTMSG_OK != ret) {
    d_->last_error = "查询成交失败";
    LOG_ERROR << d_->last_error.toStdString();
    return ret;
  }
  return RTMSG_OK;
}

// 查询操作员资金
int KSMMTradingSpi::queryAssetFund(int nAssetID) {
  auto ret = lbm_plugin_->GetFunds(nAssetID);
  if(RTMSG_OK != ret) {
    d_->last_error = "查询操作员资金失败";
    LOG_ERROR << d_->last_error.toStdString();
    return ret;
  }
  return RTMSG_OK;
}

// 查询操作员持仓
int KSMMTradingSpi::queryAssetShare(const QString& stock, int nAssetID, BOOL bZero) {
  auto ret = lbm_plugin_->GetShare(nAssetID, bZero, const_cast<char*>(stock.toStdString().data()));
  if(RTMSG_OK != ret) {
    d_->last_error = "查询操作员持仓失败";
    LOG_ERROR << d_->last_error.toStdString();
    return ret;
  }
  return RTMSG_OK;
}
///////////////////////////////////////////////////////////////////////////

// 普通行情
void KSMMTradingSpi::OnNQTick(CALLBACK_PARAM* pParam, STK_NQ_TICK* pTick) {
  auto p_tick = std::make_shared<STK_NQ_TICK>();
  memcpy(p_tick.get(), pTick, sizeof(STK_NQ_TICK));
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigNqTickArrive(p_tick);
}

// 做市行情
void KSMMTradingSpi::OnZSTick(CALLBACK_PARAM* pParam, STK_NQ_ZSXX* pTick) {
  auto p_tick = std::make_shared<STK_NQ_ZSXX>();
  memcpy(p_tick.get(), pTick, sizeof(STK_NQ_ZSXX));
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigZsTickArrive(p_tick);
}

// 委托
void KSMMTradingSpi::OnOrder(CALLBACK_PARAM* pParam, ORDER_INFO* pOrder) {
  auto p_order = std::make_shared<ORDER_INFO>();
  memcpy(p_order.get(), pOrder, sizeof(ORDER_INFO));
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigOrderArrive(p_order);
}

// 成交回报
void KSMMTradingSpi::OnTrade(CALLBACK_PARAM* pParam, MATCH_INFO* pMatch) {
  auto p_match = std::make_shared<MATCH_INFO>();
  memcpy(p_match.get(), pMatch, sizeof(MATCH_INFO));
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigMatchArrive(p_match);
}

// HQ 错误
void KSMMTradingSpi::OnHQError(CALLBACK_PARAM* pParam, char* pError) {
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigHqError(QString::fromLocal8Bit(pError));
  LOG_ERROR << pError;
}

void KSMMTradingSpi::OnMainTreadStop(CALLBACK_PARAM* pParam, char* pError) {
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigMainThreadStop(QString::fromLocal8Bit(pError));
  LOG_ERROR << pError;
}

// 错误回调
void KSMMTradingSpi::OnGetError(CALLBACK_PARAM* pParam, LBM_ERROR_INFO* pszErrorInfo) {
  auto p_sz_error = std::make_shared<LBM_ERROR_INFO>();
  memcpy(p_sz_error.get(), pszErrorInfo, sizeof(LBM_ERROR_INFO));
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigRspError(p_sz_error);
}

// 报价
void KSMMTradingSpi::OnQuote(CALLBACK_PARAM* pParam, QUOTE_BACK_INFO* pQuoteBackInfo) {
  auto p_quote = std::make_shared<QUOTE_BACK_INFO>();
  memcpy(p_quote.get(), pQuoteBackInfo, sizeof(QUOTE_BACK_INFO));
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigQuoteArrive(p_quote);
}

// 双边报价查询
void KSMMTradingSpi::OnQuoteList(CALLBACK_PARAM* pParam, QUOTE_ORDER_INFO* pQuote, int nItem) {
  SharedVecPtr<QUOTE_ORDER_INFO> p_quote_list;
  p_quote_list.reset(pQuote, nItem);
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigQuoteListArrive(p_quote_list);
}

// 报价成交
void KSMMTradingSpi::OnQuoteMatchList(CALLBACK_PARAM* pParam, QUOTE_MATCH_INFO* pMatchInfo, int nItem) {
  SharedVecPtr<QUOTE_MATCH_INFO> p_match_list;
  p_match_list.reset(pMatchInfo, nItem);
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigQuoteMatchArrive(p_match_list);
}

// 资金应答
void KSMMTradingSpi::OnFund(CALLBACK_PARAM* pParam, ASSET_FUND* pAssetFund, int nItem) {
  SharedVecPtr<ASSET_FUND> p_asset_fund_list;
  p_asset_fund_list.reset(pAssetFund, nItem);
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigQueryFundArrive(p_asset_fund_list);
}

// 持仓应答
void KSMMTradingSpi::OnShare(CALLBACK_PARAM* pParam, ASSET_SHARE* pAssetShare, int nItem) {
  SharedVecPtr<ASSET_SHARE> p_asset_share_list;
  p_asset_share_list.reset(pAssetShare, nItem);
  auto p_this = (KSMMTradingSpi*)pParam->hCallWnd;
  p_this->sigQueryShareArrive(p_asset_share_list);
}

IWKATS_NAMESPACE_END

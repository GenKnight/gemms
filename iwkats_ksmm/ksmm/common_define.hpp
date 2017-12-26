#pragma once

#include "core/iwkats_core.h"

#include <qt_windows.h>
#include <QString>
#include <memory>

#include "KSMMApi/Define.h"
#include "KSMMApi/LBM_Plugin.h"

IWKATS_NAMESPACE_BEGIN

// 最小价差
static constexpr double kMinimumQuoteSpread = 0.01;

// 最大价差
static constexpr double kMaximumAskBidRate = 0.05;

class OrderInfo;
using OrderInfoPtr = std::shared_ptr<OrderInfo>;

class MatchInfo;
using MatchInfoPtr = std::shared_ptr<MatchInfo>;

class StockObject;
using StockObjectPtr = std::shared_ptr<StockObject>;

class AutoQuoteObject;
using AutoQuoteObjectPtr = std::shared_ptr<AutoQuoteObject>;

enum OrderType {
  Buy,
  Sell,
};

// 错误类型定义
enum eErrorType {
  kGemmsSuccess = 0,
  kInputParamInvalid = -100,
  kCallLBMPluginFailed = -101,
  kStockNotExist = -102,
};

IWKATS_CORE_EXPORT QString GetErrorMsgByErrorType(int type);

/// 获取 Order 状态
IWKATS_CORE_EXPORT QString GetOrderStatusMean(const QString& key);

/// 获取证券交易所类别名称 chStkEx
IWKATS_CORE_EXPORT QString GetStkExMean(const QString& key);

/// 获取证券板块 szStkBd
IWKATS_CORE_EXPORT QString GetSzStkBdMean(const QString& key);

/// 获取股票类型 chCodeBizAction
IWKATS_CORE_EXPORT QString GetCodeBizActionMean(const QString& key);

/// 获取证券类别 chStkCls
IWKATS_CORE_EXPORT QString GetStkClsMean(const QString& key);

/// 获取资金状态 chFundStatus
IWKATS_CORE_EXPORT QString GetFundStatusMean(const QString& key);

/// 获取撤单标志 chIsWithdraw
IWKATS_CORE_EXPORT QString GetIsWithdrawMean(const QString& key);

/// 获取证券业务标志 nStkBiz
IWKATS_CORE_EXPORT QString GetBizMean(int key);

// 获取证券业务行为
IWKATS_CORE_EXPORT QString GetBizActionMean(short key);

// 格式化时间 hour:min:s:ms
IWKATS_CORE_EXPORT QString FormatQuoteTime(int quote_time);

// 格式化日期 yyyy-mm-dd
IWKATS_CORE_EXPORT QString FormatDate(int date);

// char* 每位加一个空格
IWKATS_CORE_EXPORT QString NumberToSpeech(const QString& number);

IWKATS_NAMESPACE_END

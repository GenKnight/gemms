#include "common_define.hpp"

#include <QHash>

#include "core/util.hpp"

IWKATS_NAMESPACE_BEGIN

QString GetErrorMsgByErrorType(int type)
{
  QString err_msg = QString::fromLocal8Bit("错误原因: ");
  switch(type) {
    case kInputParamInvalid:
      err_msg += QString::fromLocal8Bit("输入参数有误");
      break;
    case kCallLBMPluginFailed:
      err_msg += QString::fromLocal8Bit("调用LBM插件失败");
      break;
    case kStockNotExist:
      err_msg += QString::fromLocal8Bit("合约不存在");
      break;
    default:
      err_msg += QString::fromLocal8Bit("未知错误");
  }
  return err_msg;
}



/// 获取 Order 状态 叫买/叫卖状态 chBidOrderStatus
QString GetOrderStatusMean(const QString& key) {
  // 所有order状态
  static QHash<QString, QString> kAllOrderStatus {
    {"0", core_util::toUtf8("未报")},
    {"1", core_util::toUtf8("正报")},
    {"2", core_util::toUtf8("已报")},
    {"3", core_util::toUtf8("已报待撤")},
    {"4", core_util::toUtf8("部分待撤")},
    {"5", core_util::toUtf8("部撤")},
    {"6", core_util::toUtf8("已撤")},
    {"7", core_util::toUtf8("部分已撤")},
    {"8", core_util::toUtf8("已成")},
    {"9", core_util::toUtf8("废单")},
    {"A", core_util::toUtf8("待报")}
  };

  return kAllOrderStatus.value(key, key);
}

/// 证券交易所/交易市场  chStkEx
QString GetStkExMean(const QString& key) {
  static QHash<QString, QString> kAllStkEx {
    {"0", core_util::toUtf8("深圳交易所")},
    {"1", core_util::toUtf8("上海交易所")},
    {"2", core_util::toUtf8("北京交易所")}
  };

  return kAllStkEx.value(key, core_util::toUtf8("未知交易所"));
}

// 证券板块/交易板块 szStkBd
QString GetSzStkBdMean(const QString& key) {
  static QHash<QString, QString> kAllStkBd {
    {"00",core_util::toUtf8("深圳A股")},
    {"01",core_util::toUtf8("深圳B股")},
    {"02",core_util::toUtf8("三板(A)")},
    {"03",core_util::toUtf8("三板B")},
    {"10",core_util::toUtf8("上海A股")},
    {"11",core_util::toUtf8("上海B股")},
    {"12",core_util::toUtf8("上海个股期权")},
    {"20",core_util::toUtf8("北京股转")},
  };

  return kAllStkBd.value(key, key);
}

// 股票类型 chCodeBizAction 【没有明确释义,猜的 0 1 2】
QString GetCodeBizActionMean(const QString& key) {
  static QHash<QString, QString> kAllCode {
    {"0", core_util::toUtf8("股转协议申报")},
    {"1", core_util::toUtf8("股转做市申报")},
    {"2", core_util::toUtf8("股转限价申报")},
  };
  return kAllCode.value(key, key);
}

// 证券类别 chStkCls
QString GetStkClsMean(const QString& key) {
  static QHash<QString, QString> kAllStkCls {
    {"A",core_util::toUtf8("股票")},
    {"B",core_util::toUtf8("基金")},
    {"C",core_util::toUtf8("投资基金")},
    {"D",core_util::toUtf8("ETF基金")},
    {"E",core_util::toUtf8("LOF基金")},
    {"F",core_util::toUtf8("报价回购")},
    {"G",core_util::toUtf8("权证")},
    {"H",core_util::toUtf8("国债")},
    {"I",core_util::toUtf8("实物国债")},
    {"J",core_util::toUtf8("企业债券")},
    {"K",core_util::toUtf8("实物企债")},
    {"L",core_util::toUtf8("转换债券")},
    {"M",core_util::toUtf8("公司债")},
    {"N",core_util::toUtf8("报价转让证券")},
    {"O",core_util::toUtf8("国债回购")},
    {"P",core_util::toUtf8("企债回购")},
    {"Q",core_util::toUtf8("买断回购")},
    {"R",core_util::toUtf8("质押回购")},
    {"U",core_util::toUtf8("个股期权")},
    {"S",core_util::toUtf8("企债质押")},
    {"T",core_util::toUtf8("标准券")},
    {"V",core_util::toUtf8("股票(三板美)")},
    {"W",core_util::toUtf8("指数统计")},
    {"X",core_util::toUtf8("议案投票")},
    {"Y",core_util::toUtf8("指定交易")},
    {"Z",core_util::toUtf8("密码服务")},
    {"a",core_util::toUtf8("私募债券")},
  };

  return kAllStkCls.value(key, key);
}

// 资金状态 chFundStatus
QString GetFundStatusMean(const QString& key) {
  static QHash<QString, QString> AllFundStatus {
    {"0",core_util::toUtf8("正常")},
    {"9",core_util::toUtf8("销户")},
  };

  return AllFundStatus.value(key, key);
}

// 撤单标志 chIsWithdraw
QString GetIsWithdrawMean(const QString& key) {
  static QHash<QString, QString> AllIsWithdraw {
    {"F", core_util::toUtf8("成交")},
    {"T", core_util::toUtf8("已撤单")},
  };

  return AllIsWithdraw.value(key, key);
}

QString GetBizMean(int key)
{
  static QHash<int, QString> AllBiz {
    {400, core_util::toUtf8("买入开仓")},
    {401, core_util::toUtf8("卖出平仓")},
    {402, core_util::toUtf8("卖出开仓")},
    {403, core_util::toUtf8("买入平仓")},
    {404, core_util::toUtf8("备兑开仓")},
    {405, core_util::toUtf8("备兑平仓")},
    {406, core_util::toUtf8("认购行权")},
    {407, core_util::toUtf8("认沽行权")},
    {408, core_util::toUtf8("证券锁定(个股期权)")},
    {409, core_util::toUtf8("证券解锁(个股期权)")},
    {420, core_util::toUtf8("划入处置证券帐户")},
    {421, core_util::toUtf8("划出处置证券帐户")},
    {55000001, core_util::toUtf8("协议买入")},
    {55000002, core_util::toUtf8("协议卖出")},
    {55000003, core_util::toUtf8("做市买入")},
    {55000004, core_util::toUtf8("做市卖出")},
    {55000005, core_util::toUtf8("限价买入")},
    {55000006, core_util::toUtf8("限价卖出")},
    {55000007, core_util::toUtf8("协议互报买入")},
    {55000008, core_util::toUtf8("协议互报卖出")},
    {55000009, core_util::toUtf8("做市互报买入")},
    {55000010, core_util::toUtf8("做市互报卖出")},
    {55000011, core_util::toUtf8("定价买入")},
    {55000012, core_util::toUtf8("定价卖出")},
    {55000013, core_util::toUtf8("报价豁免")},
  };

  return AllBiz.value(key, QString::fromStdString(std::to_string(key)));
}

QString GetBizActionMean(short key)
{
  static QHash<short, QString> AllBizAction {
    {100, core_util::toUtf8("订单申报-限价委托")},
    {101, core_util::toUtf8("撤单申报")},
    {102, core_util::toUtf8("发送标志回转")},
    {103, core_util::toUtf8("合法标志回转")},
    {104, core_util::toUtf8("委托成交回转")},
    {105, core_util::toUtf8("撤单成交回转")},
    {106, core_util::toUtf8("可交易数量计算")},
    {120, core_util::toUtf8("订单申报-最优成交转价")},
    {121, core_util::toUtf8("订单申报-最优成交剩撤")},
    {122, core_util::toUtf8("订单申报-全成交或撤销")},
    {123, core_util::toUtf8("订单申报-本方最优价格")},
    {124, core_util::toUtf8("订单申报-对手最优价格")},
    {125, core_util::toUtf8("订单申报-即时成交剩撤")},
    {130, core_util::toUtf8("个股期权申报-限价GFD")},
    {131, core_util::toUtf8("个股期权申报-限价FOK")},
    {132, core_util::toUtf8("个股期权申报-市价剩转限价GFD")},
    {133, core_util::toUtf8("个股期权申报-市价FOK")},
    {134, core_util::toUtf8("个股期权申报-市价IOC")},
  };
  return AllBizAction.value(key, QString::fromStdString(std::to_string(key)));
}

QString FormatQuoteTime(int quote_time)
{
  auto ms = quote_time % 10000;

  auto us = ms % 100;
  QString strUs = QString::number(us);
  if(us < 10) {
    strUs = "0" + strUs;
  }

  ms /= 100;
  QString strMs = QString::number(ms);
  if(ms < 10) {
    strMs = "0" + strMs;
  }

  auto leftTime = quote_time / 10000;

  auto min = leftTime % 100;
  QString strMin = QString::number(min);
  if(min < 10) {
    strMin = "0" + strMin;
  }

  auto hour = leftTime / 100;
  QString strHour = QString::number(hour);
  if(hour < 10) {
    strHour = "0" + strHour;
  }

  QString str_time = QString("%1:%2:%3:%4").arg(strHour).arg(strMin).arg(strMs).arg(strUs);
  return str_time;
}

QString FormatDate(int date)
{
  auto year = date / 10000;

  auto rightTime = date % 10000;

  auto month = rightTime / 100;
  QString strMonth = QString::number(month);
  if(month < 10) {
    strMonth = "0" + strMonth;
  }

  auto day = rightTime % 100;
  QString strDay = QString::number(day);
  if(day < 10) {
    strDay = "0" + strDay;
  }

  QString str_time = QString("%1/%2/%3").arg(year).arg(strMonth).arg(strDay);
  return str_time;
}

QString NumberToSpeech(const QString& number)
{
  QString speech_stock_ = "";

  QString::const_iterator iter(number.cbegin());
  for(; iter != number.cend(); ++iter) {
    speech_stock_.append(*iter);
    speech_stock_.append(" ");
  }
  speech_stock_ = speech_stock_.trimmed();
  return speech_stock_;
}



IWKATS_NAMESPACE_END

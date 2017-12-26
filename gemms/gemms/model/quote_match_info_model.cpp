#include "quote_match_info_model.h"

#include <atltime.h>

#include "ksmm/common_define.hpp"

#include "ksmm/ksmm_trader.h"

IWKATS_NAMESPACE_BEGIN

QuoteMatchInfoModel::QuoteMatchInfoModel(QObject* parent)
  :QAbstractListModel(parent)
{
  connect(Trader::Spi(), &KSMMTradingSpi::sigQuoteMatchArrive, this, &QuoteMatchInfoModel::onQuoteMatchInfoArrive);
}

int QuoteMatchInfoModel::rowCount(const QModelIndex& parent) const
{
  return history_matchs_.size();
}

QVariant QuoteMatchInfoModel::data(const QModelIndex& index, int role) const
{

  if(!index.isValid()) {
    return QVariant();
  }

  auto row = index.row();
  if (row < 0 || row >= rowCount()) {
    return QVariant();
  }

  //std::shared_ptr<QUOTE_MATCH_INFO> p_quote_match = nullptr;
  //decltype(history_matchs_)::const_iterator iter = history_matchs_.cend();
  //for (auto i = 0; i <= row; ++i) {
  //  if (0 == i) {
  //    iter = history_matchs_.cbegin();
  //  }
  //  else {
  //    ++iter;
  //  }
  //}

  //if (iter == history_matchs_.cend()) {
  //  return QVariant();
  //}

  auto p_quote_match = history_matchs_[row];
  switch(role) {
  case KnMatchTime  : return FormatQuoteTime(p_quote_match->nMatchTime);
  case KszOrderID   : return p_quote_match->szOrderID;
  case KnAssetID    : return p_quote_match->nAssetID;
  case KnCombi_ID   : return p_quote_match->nCombi_ID;
  case KszStkCode   : return p_quote_match->szStkCode;
  case KszStkName   : return QString::fromLocal8Bit(p_quote_match->szStkName);
  case KchStkEx     : return GetStkExMean(p_quote_match->chStkEx);
  case KszStkBd     : return GetSzStkBdMean(p_quote_match->szStkBd);
  case KszStkPbu    : return p_quote_match->szStkPbu;
  case KnStkBiz     : return GetBizMean(p_quote_match->nStkBiz);
  case KnStkBizAction: return GetBizActionMean(p_quote_match->nStkBizAction);
  case KdOrderPrice : return p_quote_match->dOrderPrice;
  case KnOrderQty   : return p_quote_match->nOrderQty;
  case KchIsWithdraw: return GetIsWithdrawMean(p_quote_match->chIsWithdraw);
  case KdMatchedPrice: return p_quote_match->dMatchedPrice;
  case KnMatchedQty : return p_quote_match->nMatchedQty;
  case KdMatchedAmt : return p_quote_match->dMatchedAmt;
  case KnTradeDate  : return FormatDate(p_quote_match->nTradeDate);
  case KszMatchedSN : return p_quote_match->szMatchedSN;
  default:
    return QVariant();
  }
  return QVariant();
}

QHash<int, QByteArray> QuoteMatchInfoModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[KnMatchTime]      = core_util::toUtf8("成交时间");
  roles[KszOrderID]       = core_util::toUtf8("委托合同序号");
  roles[KnAssetID]        = core_util::toUtf8("资产单元ID");
  roles[KnCombi_ID]       = core_util::toUtf8("组合ID");
  roles[KszStkCode]       = core_util::toUtf8("证券代码");
  roles[KszStkName]       = core_util::toUtf8("证券名称");
  roles[KchStkEx]         = core_util::toUtf8("证券交易所");
  roles[KszStkBd]         = core_util::toUtf8("交易板块");
  roles[KszStkPbu]        = core_util::toUtf8("交易单元");
  roles[KnStkBiz]         = core_util::toUtf8("证券业务");
  roles[KnStkBizAction]    = core_util::toUtf8("证券业务行为");
  roles[KdOrderPrice]     = core_util::toUtf8("委托价格");
  roles[KnOrderQty]       = core_util::toUtf8("委托数量");
  roles[KchIsWithdraw]    = core_util::toUtf8("撤单状态");
  roles[KdMatchedPrice]    = core_util::toUtf8("成交价格");
  roles[KnMatchedQty]     = core_util::toUtf8("成交数量");
  roles[KdMatchedAmt]     = core_util::toUtf8("成交金额");
  roles[KnTradeDate]      = core_util::toUtf8("交易日期");
  roles[KszMatchedSN]     = core_util::toUtf8("成交编号");

  return roles;
}

QStringList QuoteMatchInfoModel::customRoleNames()
{
  QStringList rolesList;
  rolesList << core_util::toUtf8("证券代码")
            << core_util::toUtf8("证券名称")
            << core_util::toUtf8("交易日期")
            << core_util::toUtf8("成交时间")
            << core_util::toUtf8("证券业务行为")
            << core_util::toUtf8("委托合同序号")
            << core_util::toUtf8("成交编号")
            << core_util::toUtf8("委托价格")
            << core_util::toUtf8("委托数量")
            << core_util::toUtf8("撤单状态")
            << core_util::toUtf8("成交价格")
            << core_util::toUtf8("成交数量")
            << core_util::toUtf8("成交金额")
            << core_util::toUtf8("证券交易所")
            << core_util::toUtf8("交易板块")
            << core_util::toUtf8("交易单元")
            << core_util::toUtf8("证券业务")
            << core_util::toUtf8("资产单元ID")
            << core_util::toUtf8("组合ID");
  return rolesList;
}

void QuoteMatchInfoModel::refresh(const QString& stock)
{
  beginResetModel();
  history_matchs_.clear();
  endResetModel();

  stock_ = stock;

  int nBegTime = 1;
  CTime tm = CTime::GetCurrentTime();
  int nEndTime = tm.GetHour()*1000000+tm.GetMinute()*10000+tm.GetSecond()*100;
  int nBegDate = 1;
  int nEndDate = tm.GetYear()*10000+tm.GetMonth()*100+tm.GetDay();

  if(stock.isEmpty()) {
    std::vector<ASSET_CODE*> asset_codes;
    Trader::Spi()->GetAssetCodes(asset_codes);
    for(auto& asset_code : asset_codes) {
      Trader::Api()->listQuoteMatch(asset_code->szStkCode, nBegTime, nEndTime, nBegDate, nEndDate);
    }
  } else {
    Trader::Api()->listQuoteMatch(stock, nBegTime, nEndTime, nBegDate, nEndDate);
  }
}

void QuoteMatchInfoModel::onQuoteMatchInfoArrive(SharedVecPtr<QUOTE_MATCH_INFO> pQuoteMatch)
{
  beginResetModel();
  auto it = pQuoteMatch.data()->cbegin();
  for(; it != pQuoteMatch.data()->cend(); ++it) {
    if(stock_.isEmpty() || 0 == strcmp(stock_.toStdString().data(), (*it)->szStkCode)) {
      const auto& item = (*it);

      if (valid_checks_.contains(item->szMatchedSN)) {
        // 当前model中已经有，更新
        auto update_row = valid_checks_.value(item->szMatchedSN);
        history_matchs_[update_row] = item;
        emit dataChanged(index(update_row), index(update_row));
      }
      else {
        // 插入
        auto insert_row = rowCount();
        beginInsertRows(QModelIndex(), insert_row, insert_row);
        valid_checks_.insert(item->szMatchedSN, insert_row);
        history_matchs_.push_back(item);
        endInsertRows();
      }
    }
  }

  endResetModel();

}

IWKATS_NAMESPACE_END

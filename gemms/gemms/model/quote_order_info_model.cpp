#include "quote_order_info_model.h"

#include "ksmm/ksmm_trader.h"

#include <atltime.h>
#include "ksmm/common_define.hpp"
#include "stock_list_model.h"

IWKATS_NAMESPACE_BEGIN

QuoteOrderInfoModel::QuoteOrderInfoModel(QObject* parent)
  :QAbstractListModel(parent)
{
  connect(Trader::Spi(), &KSMMTradingSpi::sigQuoteListArrive, this, &QuoteOrderInfoModel::onQuoteOrderInfoArrive);
}

int QuoteOrderInfoModel::rowCount(const QModelIndex& parent) const
{
  return history_orders_.size();
}

QVariant QuoteOrderInfoModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()) {
    return QVariant();
  }

  auto row = index.row();
  if(row < 0 || row >= rowCount()) {
    return QVariant();
  }


  auto p_quote_order = history_orders_[row];

  ASSET_CODE* asset_code = Trader::Spi()->GetAssetCode(p_quote_order->szStkCode);

  switch(role) {
    case  KnAssetID         :   return p_quote_order->nAssetID;
    case  KszStkName        :   return QString::fromLocal8Bit(asset_code->szStkName);
    case  KszStkCode        :   return p_quote_order->szStkCode;
    case  KszOrderID        :   return p_quote_order->szOrderID;
    case  KdAskPrice        :   return p_quote_order->dAskPrice;
    case  KnAskWtVolume     :   return p_quote_order->nAskWtVolume;
    case  KnAskVolume       :   return p_quote_order->nAskVolume;
    case  KchAskOrderStatus :   return GetOrderStatusMean(p_quote_order->chAskOrderStatus);
    case  KnAskMatchVol     :   return p_quote_order->nAskMatchVol;
    case  KnAskCancelVol    :   return p_quote_order->nAskCancelVol;
    case  KdBidPrice        :   return p_quote_order->dBidPrice;
    case  KnBidWtVolume     :   return p_quote_order->nBidWtVolume;
    case  KnBidVolume       :   return p_quote_order->nBidVolume;
    case  KchBidOrderStatus :   return GetOrderStatusMean(p_quote_order->chBidOrderStatus);
    case  KnBidMatchVol     :   return p_quote_order->nBidMatchVol;
    case  KnBidCancelVol    :   return p_quote_order->nBidCancelVol;
    case  KnQuoteDate       :   return FormatDate(p_quote_order->nQuoteDate);
    case  KnQuoteTime       :   return FormatQuoteTime(p_quote_order->nQuoteTime);
    default:
      return QVariant();
  }
  return QVariant();
}

QHash<int, QByteArray> QuoteOrderInfoModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[KnAssetID]          = core_util::toUtf8("资产单元ID");
  roles[KszStkName]         = core_util::toUtf8("证券名称");
  roles[KszStkCode]         = core_util::toUtf8("证券代码");
  roles[KszOrderID]         = core_util::toUtf8("委托合同序号");
  roles[KdAskPrice]         = core_util::toUtf8("叫卖价");
  roles[KnAskWtVolume]      = core_util::toUtf8("叫卖数量");
  roles[KnAskVolume]        = core_util::toUtf8("叫卖挂单");
  roles[KchAskOrderStatus]  = core_util::toUtf8("叫卖委托状态");
  roles[KnAskMatchVol]      = core_util::toUtf8("叫卖成交数量");
  roles[KnAskCancelVol]     = core_util::toUtf8("叫卖撤单数量");
  roles[KdBidPrice]         = core_util::toUtf8("叫买价");
  roles[KnBidWtVolume]      = core_util::toUtf8("叫买数量");
  roles[KnBidVolume]        = core_util::toUtf8("叫买挂单");
  roles[KchBidOrderStatus]  = core_util::toUtf8("叫买委托状态");
  roles[KnBidMatchVol]      = core_util::toUtf8("叫买成交数量");
  roles[KnBidCancelVol]     = core_util::toUtf8("叫买撤单数量");
  roles[KnQuoteDate]        = core_util::toUtf8("报价日期");
  roles[KnQuoteTime]        = core_util::toUtf8("报价时间");

  return roles;
}

QStringList QuoteOrderInfoModel::customRoleNames()
{
  QStringList rolesList{
    core_util::toUtf8("证券代码"),
        core_util::toUtf8("证券名称"),
        core_util::toUtf8("叫买委托状态"),
        core_util::toUtf8("叫卖委托状态"),
        core_util::toUtf8("叫卖成交数量"),
        core_util::toUtf8("叫买成交数量"),
        core_util::toUtf8("报价日期"),
        core_util::toUtf8("报价时间"),
        core_util::toUtf8("叫卖价"),
        core_util::toUtf8("叫卖数量"),
        core_util::toUtf8("叫卖挂单"),
        core_util::toUtf8("叫卖撤单数量"),
        core_util::toUtf8("叫买价"),
        core_util::toUtf8("叫买数量"),
        core_util::toUtf8("叫买挂单"),
        core_util::toUtf8("叫买撤单数量"),
        core_util::toUtf8("资产单元ID"),
        core_util::toUtf8("委托合同序号")
  };
  return rolesList;
}

void QuoteOrderInfoModel::refresh(const QString& stock)
{
  // 清空上次
  beginResetModel();
  valid_checks_.clear();
  history_orders_.clear();
  endResetModel();

  // 记录刷新的股票
  stock_ = stock;

  // 查询
  int nBegTime = 1;
  CTime tm = CTime::GetCurrentTime();
  int nEndTime = tm.GetHour()*1000000+tm.GetMinute()*10000+tm.GetSecond()*100;
  int nBegDate = 1;
  int nEndDate = tm.GetYear()*10000+tm.GetMonth()*100+tm.GetDay();

  if(stock.isEmpty()) {
    std::vector<ASSET_CODE*> asset_codes;
    Trader::Spi()->GetAssetCodes(asset_codes);
    for(auto& asset_code : asset_codes) {
      Trader::Api()->listQuote(asset_code->szStkCode, nBegTime, nEndTime, nBegDate, nEndDate);
    }
  } else {
    Trader::Api()->listQuote(stock, nBegTime, nEndTime, nBegDate, nEndDate);
  }
}

void QuoteOrderInfoModel::cancelOrder(int row)
{
  if(row < 0 || row >= rowCount()) {
    return;
  }

  std::shared_ptr<QUOTE_ORDER_INFO> quote_order_info = history_orders_.at(row);

  // TODO 判断是否能撤单

  Trader::Api()->cancelOrder(quote_order_info->szOrderID);
}

void QuoteOrderInfoModel::onQuoteOrderInfoArrive(SharedVecPtr<QUOTE_ORDER_INFO> pQuoteOrder)
{
  auto it = pQuoteOrder.data()->cbegin();
  for(; it != pQuoteOrder.data()->cend(); ++it) {
    if(stock_.isEmpty() || 0 == strcmp(stock_.toStdString().data(), (*it)->szStkCode)) {
      const auto& item = (*it);
      if(valid_checks_.contains(item->szOrderID)) {
        // 当前model中已经有该委托，更新
        auto update_row = valid_checks_.value(item->szOrderID);
        history_orders_[update_row] = item;
        emit dataChanged(index(update_row), index(update_row));
      } else {
        // 插入
        auto insert_row = rowCount();
        beginInsertRows(QModelIndex(), insert_row, insert_row);
        valid_checks_.insert(item->szOrderID, insert_row);
        history_orders_.push_back(item);
        endInsertRows();
      }
    }
  }
}


IWKATS_NAMESPACE_END

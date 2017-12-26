#include "match_info_model.h"
#include "ksmm/ksmm_trader.h"

#include "ksmm/match/match_manager.h"

#include "ksmm/match/match_info.h"
#include "ksmm/common_define.hpp"

IWKATS_NAMESPACE_BEGIN

MatchInfoModel::MatchInfoModel(QObject* parent)
  :QAbstractListModel(parent)
{
  connect(MatchManager::GetPtr(), &MatchManager::sigMatchArrive, this, &MatchInfoModel::onMatchInfoArrive);
}

int MatchInfoModel::rowCount(const QModelIndex& parent) const
{
  return matchs_.size();
}

QVariant MatchInfoModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()) {
    return QVariant();
  }

  auto row = index.row();
  if (row < 0 || row >= rowCount()) {
    return QVariant();
  }

  auto p_match_info = matchs_.at(row)->match_ptr();
  switch(role) {
    case KszOrderID     :  return p_match_info->szOrderID;
    case KnAssetID      :  return p_match_info->nAssetID;
    case KszStkCode     :  return p_match_info->szStkCode;
    case KdAskOrderPrice:  return p_match_info->dAskOrderPrice;
    case KdBidOrderPrice:  return p_match_info->dBidOrderPrice;
    case KnAskOrderQty  :  return p_match_info->nAskOrderQty  ;
    case KnBidOrderQty  :  return p_match_info->nBidOrderQty  ;
    case KchIsWithdraw  :  return GetIsWithdrawMean(p_match_info->chIsWithdraw);
    case KnAskMatchedQty:  return p_match_info->nAskMatchedQty;
    case KnBidMatchedQty:  return p_match_info->nBidMatchedQty;
    case KnMatchDate    :  return FormatDate(p_match_info->nMatchDate);
    case KnMatchTime    :  return FormatQuoteTime(p_match_info->nMatchTime);
    default:
      return QVariant();
  }
  return QVariant();
}

QHash<int, QByteArray> MatchInfoModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[KszOrderID]      = core_util::toUtf8("委托合同序号");
  roles[KnAssetID]       = core_util::toUtf8("资产单元ID");
  roles[KszStkCode]      = core_util::toUtf8("证券代码");
  roles[KdAskOrderPrice] = core_util::toUtf8("叫卖价格");
  roles[KdBidOrderPrice] = core_util::toUtf8("叫买价格");
  roles[KnAskOrderQty]   = core_util::toUtf8("叫卖委托数量");
  roles[KnBidOrderQty]   = core_util::toUtf8("叫买委托数量");
  roles[KchIsWithdraw]   = core_util::toUtf8("撤单状态");
  roles[KnAskMatchedQty] = core_util::toUtf8("叫卖成交数量");
  roles[KnBidMatchedQty] = core_util::toUtf8("叫买成交数量");
  roles[KnMatchDate]     = core_util::toUtf8("成交日期");
  roles[KnMatchTime]     = core_util::toUtf8("成交时间");

  return roles;
}

QStringList MatchInfoModel::customRoleNames()
{
  QStringList roleLists;
  roleLists << core_util::toUtf8("成交日期")
            << core_util::toUtf8("成交时间")
            << core_util::toUtf8("撤单状态")
            << core_util::toUtf8("叫卖价格")
            << core_util::toUtf8("叫买价格")
            << core_util::toUtf8("叫卖委托数量")
            << core_util::toUtf8("叫买委托数量")
            << core_util::toUtf8("叫卖成交数量")
            << core_util::toUtf8("叫买成交数量")
            << core_util::toUtf8("证券代码")
            << core_util::toUtf8("资产单元ID")
            << core_util::toUtf8("委托合同序号");
  return roleLists;
}

void MatchInfoModel::setStock(const QString& code)
{
  beginResetModel();
  valid_checks_.clear();
  matchs_.clear();

  stock_ = code;

  if(stock_.isEmpty()) {
    MatchManager::GetPtr()->GetAllMatchs(matchs_);
  } else {
    MatchManager::GetPtr()->GetAllMatchByStock(stock_, matchs_);
  }

  for(auto i = 0; i < matchs_.size(); ++i) {
    const auto& match_ptr = matchs_.at(i);
    valid_checks_.insert(match_ptr->szOrderID(), i);
  }

  endResetModel();
}

void MatchInfoModel::onMatchInfoArrive(MatchInfoPtr match_ptr)
{
  if(stock_.isEmpty() || 0 == strcmp(stock_.toStdString().data(), match_ptr->GetMatchInfoPtr()->szStkCode)) {
    QString order_id = match_ptr->GetMatchInfoPtr()->szOrderID;

    if(valid_checks_.contains(order_id)) {
      // 当前model中已经有该委托，更新
      auto update_row = valid_checks_.value(order_id);
      matchs_[update_row] = match_ptr;
      emit dataChanged(index(update_row), index(update_row));
    } else {
      // 插入
      beginResetModel();

      QHashIterator<QString, int> it(valid_checks_);
      while(it.hasNext()) {
        it.next();
        valid_checks_[it.key()] = it.value() + 1;
      }

      valid_checks_.insert(order_id, 0);
      matchs_.push_front(match_ptr);
      endResetModel();
    }
  }
}

IWKATS_NAMESPACE_END



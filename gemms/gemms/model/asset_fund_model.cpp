#include "asset_fund_model.h"

#include "ksmm/common_define.hpp"

#include "ksmm/ksmm_trader.h"

IWKATS_NAMESPACE_BEGIN
// 查询资金
AssetFundModel::AssetFundModel(QObject* parent)
  :QAbstractListModel(parent)
{
  connect(Trader::Spi(), &KSMMTradingSpi::sigQueryFundArrive, this, &AssetFundModel::onAssetFundArrive);
}

int AssetFundModel::rowCount(const QModelIndex& parent) const
{
  return asset_funds_.size();
}

QVariant AssetFundModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()) {
    return QVariant();
  }

  auto row = index.row();
  if(row < 0 || row >= rowCount()) {
    return QVariant();
  }

  auto p_asset_fund = asset_funds_[row];
  switch (role) {
  case KszProjectName   :  return core_util::toUtf8(p_asset_fund->szProjectName);
  case KnAssetId        :  return p_asset_fund->nAssetId     ;
  case KdMaxMoney       :  return p_asset_fund->dMaxMoney    ;
  case KdMinMoney       :  return p_asset_fund->dMinMoney    ;
  case KdFundPrebln     :  return QString::number(p_asset_fund->dFundPrebln, 'f', 2);
  case KdFundBln        :  return QString::number(p_asset_fund->dFundBln, 'f', 0);
  case KdFundAvl        :  return QString::number(p_asset_fund->dFundAvl, 'f', 0);
  case KdFundFrz        :  return QString::number(p_asset_fund->dFundFrz, 'f', 0);
  case KdFundUfz        :  return QString::number(p_asset_fund->dFundUfz, 'f', 0);
  case KdInterest       :  return QString::number(p_asset_fund->dInterest, 'f', 2);
  case KdMktVal         :  return QString::number(p_asset_fund->dMktVal, 'f', 0);
  case KdLastFundClr    :  return QString::number(p_asset_fund->dLastFundClr, 'f', 0);
  case KchFundStatus    :  return GetFundStatusMean(p_asset_fund->chFundStatus);
  case KdPaylater       :  return QString::number(p_asset_fund->dPaylater, 'f', 0);
  default:
    return QVariant();
  }
  return QVariant();
}

QHash<int, QByteArray> AssetFundModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[KszProjectName]   = core_util::toUtf8("项目名称");
  roles[KnAssetId]        = core_util::toUtf8("资产单元ID");
  roles[KdMaxMoney]       = core_util::toUtf8("资金上限");
  roles[KdMinMoney]       = core_util::toUtf8("资金下限");
  roles[KdFundPrebln]     = core_util::toUtf8("资金昨日余额");
  roles[KdFundBln]        = core_util::toUtf8("资金当前余额");
  roles[KdFundAvl]        = core_util::toUtf8("资金可用金额");
  roles[KdFundFrz]        = core_util::toUtf8("资金冻结金额");
  roles[KdFundUfz]        = core_util::toUtf8("资金解冻金额");
  roles[KdInterest]       = core_util::toUtf8("利息");
  roles[KdLastFundClr]    = core_util::toUtf8("上次清算金额");
  roles[KchFundStatus]    = core_util::toUtf8("资金状态");
  roles[KdPaylater]       = core_util::toUtf8("垫付资金");

  return roles;
}

QStringList AssetFundModel::customRoleNames()
{
  QMap<int, QString> res;
  QHashIterator<int, QByteArray> i(roleNames());
  while(i.hasNext()) {
    i.next();
    if(i.key() > Qt::UserRole) {
      res[i.key()] = i.value();
    }
  }
  return res.values();
}

void AssetFundModel::refresh()
{
  beginResetModel();
  asset_funds_.clear();
  valid_checks_.clear();
  endResetModel();

  // 重新查询所有资产单元的资金
  Trader::Api()->queryAssetFund();
}

void AssetFundModel::onAssetFundArrive(SharedVecPtr<ASSET_FUND> pAssetFund)
{
  auto it = pAssetFund.data()->begin();
  for(; it != pAssetFund.data()->end(); ++it) {

    // 如果当前没有此股票的持仓，则插入，否则视为重复
    const auto& item = (*it);

    if(valid_checks_.contains(item->nAssetId)) {
       // 当前model中已经有该委托，更新
      auto update_row = valid_checks_.value(item->nAssetId);
      asset_funds_[update_row] = item;
      emit dataChanged(index(update_row), index(update_row));
    } else {
      // 插入
      auto insert_row = rowCount();
      beginInsertRows(QModelIndex(), insert_row, insert_row);
      valid_checks_.insert(item->nAssetId, insert_row);
      asset_funds_.push_back(item);
      endInsertRows();
    }

  }
}

IWKATS_NAMESPACE_END

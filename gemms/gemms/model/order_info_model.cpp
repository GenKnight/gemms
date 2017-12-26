#include "order_info_model.h"

#include <algorithm>

#include "ksmm/order/order_manager.h"
#include "ksmm/order/order_info.h"
#include "ksmm/common_define.hpp"

IWKATS_NAMESPACE_BEGIN

OrderInfoModel::OrderInfoModel(QObject* parent)
  :QAbstractListModel(parent)
{
  connect(OrderManager::GetPtr(), &OrderManager::sigOrderArrive, this, &OrderInfoModel::onOrderInfoArrive);
}

int OrderInfoModel::rowCount(const QModelIndex& parent) const
{
  return orders_.size();
}

QVariant OrderInfoModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()) {
    return QVariant();
  }

  auto row = index.row();
  if(row < 0 || row >= rowCount()) {
    return QVariant();
  }

  auto p_order_info = orders_.at(row)->GetOrderInfoPtr();
  switch(role) {
    case KszStkCode            :  return p_order_info->szStkCode   ;
    case KszOrderID            :  return p_order_info->szOrderID   ;
    case KdAskPrice            :  return p_order_info->dAskPrice   ;
    case KnAskWtVolume         :  return p_order_info->nAskWtVolume;
    case KnAskVolume           :  return p_order_info->nAskVolume  ;
    case KchAskOrderStatus     :  return GetOrderStatusMean(p_order_info->chAskOrderStatus);
    case KnAskMatchVol         :  return p_order_info->nAskMatchVol ;
    case KnAskCancelVol        :  return p_order_info->nAskCancelVol;
    case KdBidPrice            :  return p_order_info->dBidPrice    ;
    case KnBidWtVolume         :  return p_order_info->nBidWtVolume ;
    case KnBidVolume           :  return p_order_info->nBidVolume   ;
    case KchBidOrderStatus     :  return GetOrderStatusMean(p_order_info->chBidOrderStatus);
    case KnBidMatchVol         :  return p_order_info->nBidMatchVol ;
    case KnBidCancelVol        :  return p_order_info->nBidCancelVol;
    case KnQuoteDate           :  return FormatDate(p_order_info->nQuoteDate);
    case KnQuoteTime           :  return FormatQuoteTime(p_order_info->nQuoteTime);
    default:
      return QVariant();
  }
  return QVariant();
}

QHash<int, QByteArray> OrderInfoModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[KszStkCode]          = core_util::toUtf8("证券代码");
  roles[KszOrderID]          = core_util::toUtf8("委托合同序号");
  roles[KdAskPrice]          = core_util::toUtf8("叫卖价");
  roles[KnAskWtVolume]       = core_util::toUtf8("叫卖数量");
  roles[KnAskVolume]         = core_util::toUtf8("叫卖挂单");
  roles[KchAskOrderStatus]   = core_util::toUtf8("叫卖委托状态");
  roles[KnAskMatchVol]       = core_util::toUtf8("叫卖成交数量");
  roles[KnAskCancelVol]      = core_util::toUtf8("叫卖撤单数量");
  roles[KdBidPrice]          = core_util::toUtf8("叫买价");
  roles[KnBidWtVolume]       = core_util::toUtf8("叫买数量");
  roles[KnBidVolume]         = core_util::toUtf8("叫买挂单");
  roles[KchBidOrderStatus]   = core_util::toUtf8("叫买委托状态");
  roles[KnBidMatchVol]       = core_util::toUtf8("叫买成交数量");
  roles[KnBidCancelVol]      = core_util::toUtf8("叫买撤单数量");
  roles[KnQuoteDate]         = core_util::toUtf8("报价日期");
  roles[KnQuoteTime]         = core_util::toUtf8("报价时间");

  return roles;
}

QStringList OrderInfoModel::customRoleNames()
{
  QStringList roleLists;
  roleLists << core_util::toUtf8("叫买委托状态")
            << core_util::toUtf8("叫卖委托状态")
            << core_util::toUtf8("叫卖成交数量")
            << core_util::toUtf8("叫买成交数量")
            << core_util::toUtf8("委托合同序号")
            << core_util::toUtf8("叫卖撤单数量")
            << core_util::toUtf8("叫买撤单数量")
            << core_util::toUtf8("报价日期")
            << core_util::toUtf8("报价时间")
            << core_util::toUtf8("叫卖价")
            << core_util::toUtf8("叫卖数量")
            << core_util::toUtf8("叫卖挂单")
            << core_util::toUtf8("叫买价")
            << core_util::toUtf8("叫买数量")
            << core_util::toUtf8("叫买挂单")
            << core_util::toUtf8("证券代码");
  return roleLists;
}

void OrderInfoModel::setStock(const QString& code)
{

  // 获取历史该股票的委托回报
  beginResetModel();
  orders_.clear();
  valid_checks_.clear();

  stock_ = code;

  if(stock_.isEmpty()) {
    OrderManager::GetPtr()->GetAllOrders(orders_);
  } else {
    OrderManager::GetPtr()->GetAllOrdersByStock(stock_, orders_);
  }

  for(auto i = 0; i < orders_.size(); ++i) {
    const auto& order_ptr = orders_.at(i);
    valid_checks_.insert(order_ptr->orderId(), i);
  }

  endResetModel();
}

void OrderInfoModel::cancelOrder(int row)
{
  if(row < 0 || row >= rowCount()) {
    return;
  }

  auto order_ptr = orders_.at(row);
  order_ptr->cancelOrder();
}

void OrderInfoModel::onOrderInfoArrive(OrderInfoPtr order_info_ptr)
{
  QString order_id = order_info_ptr->GetOrderInfoPtr()->szOrderID;
  if(stock_.isEmpty() || 0 == strcmp(stock_.toStdString().data(), order_info_ptr->GetOrderInfoPtr()->szStkCode)) {
    if(valid_checks_.contains(order_id)) {
      // 当前model中已经有该委托，更新
      auto update_row = valid_checks_.value(order_id);
      orders_[update_row] = order_info_ptr;
      emit dataChanged(index(update_row), index(update_row));
    } else {
      // 插入
      beginResetModel();
      QHashIterator<QString, int> i(valid_checks_);
      while(i.hasNext()) {
        i.next();
        valid_checks_[i.key()] = i.value() + 1;
      }
      valid_checks_.insert(order_id, 0);
      orders_.push_front(order_info_ptr);
      endResetModel();
    }
  }
}

IWKATS_NAMESPACE_END

#pragma once

#include "core/iwkats_core.h"
#include "core/util.hpp"
#include "KSMMApi/Define.h"

#include <QMap>
#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

// 历史委托 (做市委托单查询)
class QuoteOrderInfoModel : public QAbstractListModel
{
  Q_OBJECT
  Q_ENUMS(eQuoteOrderInfo)
public:
  enum eQuoteOrderInfo {
    BeginUserRole =  Qt::UserRole,
    KnAssetID,              // 资产单元ID
    KszStkCode,             // 证券代码
    KszStkName,             // 证券名称
    KszOrderID,             // 委托合同序号
    KdAskPrice,             // 叫卖价
    KnAskWtVolume,          // 叫卖数量
    KnAskVolume,            // 叫卖挂单
    KchAskOrderStatus,      // 叫卖委托状态
    KnAskMatchVol,          // 叫卖成交数量
    KnAskCancelVol,         // 叫卖撤单数量
    KdBidPrice,             // 叫买价
    KnBidWtVolume,          // 叫买数量
    KnBidVolume,            // 叫买挂单
    KchBidOrderStatus,      // 叫买委托状态
    KnBidMatchVol,          // 叫买成交数量
    KnBidCancelVol,         // 叫买撤单数量
    KnQuoteDate,            // 报价日期
    KnQuoteTime,            // 报价时间
    EndUserRole
  };
public:
  explicit QuoteOrderInfoModel(QObject* parent = Q_NULLPTR);
  ~QuoteOrderInfoModel() {}
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames()const override;
  Q_INVOKABLE QStringList customRoleNames();

  // 刷新
  Q_INVOKABLE void refresh(const QString& stock);

  // 撤单
  Q_INVOKABLE void cancelOrder(int row);

public slots:
  void onQuoteOrderInfoArrive(SharedVecPtr<QUOTE_ORDER_INFO> pQuoteOrder);

private:
  QString stock_;

  // 订单所在的行
  QMap<QString, int> valid_checks_;
  // 所有订单
  QList<std::shared_ptr<QUOTE_ORDER_INFO>> history_orders_;
};

IWKATS_NAMESPACE_END

#pragma once

#include "ksmm/common_define.hpp"
#include "core/iwkats_core.h"
#include "core/util.hpp"
#include "KSMMApi/Define.h"

#include <QSet>
#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

// 成交回报
class OrderInfoModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(QString stock READ stock WRITE setStock)
  Q_ENUMS(eOrderInfo)
public:
  enum eOrderInfo {
    BeginUserRole = Qt::UserRole,
    KszStkCode,                    // 证券代码
    KszOrderID,                    // 委托合同序号
    KdAskPrice,                    // 叫卖价
    KnAskWtVolume,                 // 叫卖数量委托合同序号
    KnAskVolume,                   // 叫卖挂单
    KchAskOrderStatus,             // 叫卖委托状态
    KnAskMatchVol,                 // 叫卖成交数量
    KnAskCancelVol,                // 叫卖撤单数量
    KdBidPrice,                    // 叫买价
    KnBidWtVolume,                 // 叫买数量
    KnBidVolume,                   // 叫买挂单
    KchBidOrderStatus,             // 叫买委托状态
    KnBidMatchVol,                 // 叫买成交数量
    KnBidCancelVol,                // 叫买撤单数量
    KnQuoteDate,                   // 报价日期
    KnQuoteTime,                   // 报价时间
    EndUserRole
  };

  explicit OrderInfoModel(QObject *parent = Q_NULLPTR);
  virtual ~OrderInfoModel() {}

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames()const override;
  Q_INVOKABLE QStringList customRoleNames();

  QString stock() const { return stock_; }
  void setStock(const QString& code);

  /// 撤单
  Q_INVOKABLE void cancelOrder(int row);

public slots:
  void onOrderInfoArrive(OrderInfoPtr order_info_ptr);

private:
  QString stock_{""};
  QHash<QString, int> valid_checks_;
  QList<OrderInfoPtr> orders_;

};

IWKATS_NAMESPACE_END

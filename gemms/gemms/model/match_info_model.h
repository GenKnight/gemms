#pragma once

#include "core/iwkats_core.h"
#include "core/util.hpp"
#include "ksmm/common_define.hpp"

#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

//委托回报
class MatchInfoModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(QString stock READ stock WRITE setStock)
public:
  enum eMatchInfo {
    BeginUserRole = Qt::UserRole,
    KszOrderID,                                // 委托合同序号
    KnAssetID,                                 // 资产单元ID
    KszStkCode,                                // 证券代码
    KdAskOrderPrice,                           // 叫卖价格
    KdBidOrderPrice,                           // 叫买价格
    KnAskOrderQty,                             // 叫卖委托数量
    KnBidOrderQty,                             // 叫买委托数量
    KchIsWithdraw,                             // 撤单标志
    KnAskMatchedQty,                           // 叫卖成交数量
    KnBidMatchedQty,                           // 叫买成交数量
    KnMatchDate,                               // 成交日期 fengwc alger  7.27
    KnMatchTime,                               // 成交时间 fengwc alger 7.27
    EndUserRole
  };
public:
  explicit MatchInfoModel(QObject *parent = Q_NULLPTR);
  ~MatchInfoModel() {}

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  Q_INVOKABLE QStringList customRoleNames();

  QString stock() const { return stock_; }
  void setStock(const QString& code);

public slots:
  void onMatchInfoArrive(MatchInfoPtr match_ptr);

private:
  QString stock_{""};

  QHash<QString, int> valid_checks_;
  QList<MatchInfoPtr> matchs_;
};

IWKATS_NAMESPACE_END

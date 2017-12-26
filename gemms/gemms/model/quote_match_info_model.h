#pragma once

#include "core/iwkats_core.h"
#include "core/util.hpp"
#include "ksmm/common_define.hpp"

#include <set>
#include <QSet>
#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

class QuoteMatchInfoCmp {
public:
  bool operator()(const std::shared_ptr<QUOTE_MATCH_INFO>& lhs,
                  const std::shared_ptr<QUOTE_MATCH_INFO>& rhs) {
    return (lhs->nTradeDate > rhs->nTradeDate
            || (lhs->nTradeDate == rhs->nTradeDate
                && lhs->nMatchTime > rhs->nMatchTime));
  }
};

class QuoteMatchInfoModel : public QAbstractListModel
{
  Q_OBJECT
public:
  enum eQuoteMatchInfo {
    BeginUserRole = Qt::UserRole,
    KnMatchTime,             // 成交时间
    KszOrderID,              // 委托合同序号
    KnAssetID,               // 资产单元ID
    KnCombi_ID,              // 组合ID
    KszStkCode,              // 证券代码
    KszStkName,              // 证券名称
    KchStkEx,                // 证券交易所
    KszStkBd,                // 交易板块
    KszStkPbu,               // 交易单元
    KnStkBiz,                // 证券业务
    KnStkBizAction,          // 证券业务行为
    KdOrderPrice,            // 委托价格
    KnOrderQty,              // 委托数量
    KchIsWithdraw,           // 撤单标志
    KdMatchedPrice,          // 成交价格
    KnMatchedQty,            // 成交数量
    KdMatchedAmt,            // 成交金额
    KnTradeDate,             // 交易日期
    KszMatchedSN,            // 成交编号
    EndUserRole
  };
public:
  explicit QuoteMatchInfoModel(QObject* parent = Q_NULLPTR);
  ~QuoteMatchInfoModel() {}
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames()const override;
  Q_INVOKABLE QStringList customRoleNames();

  Q_INVOKABLE void refresh(const QString& stock);

public slots:
  void onQuoteMatchInfoArrive(SharedVecPtr<QUOTE_MATCH_INFO> pQuoteMatch);

private:
  QString stock_;
  QHash<QString, int> valid_checks_;
  QList<std::shared_ptr<QUOTE_MATCH_INFO>> history_matchs_;
};

IWKATS_NAMESPACE_END

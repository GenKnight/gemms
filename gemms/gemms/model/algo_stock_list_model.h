#pragma once

#include "core/iwkats_core.h"
#include "ksmm/common_define.hpp"

#include <QAbstractListModel>
#include <QVector>

IWKATS_NAMESPACE_BEGIN

class AlgoStockListModel : public QAbstractListModel {
  Q_OBJECT
  Q_ENUMS(AlgoStockType)
public:
  enum AlgoStockType {
    KszStkCode = Qt::UserRole + 1,      ///< 证券代码
    KszStkName,                         ///< 证券名称
    KchStkEx,                           ///< 证券交易所
    KszStkBd,                           ///< 证券板块
    KchCodeBizAction,                   ///< 股票类型
    KszCombiName,                       ///< 组合名称
    KszAssetName,                       ///< 资产单元名称
    KisRunning,
  };

  AlgoStockListModel(QObject* parent = nullptr);

  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE AutoQuoteObject* getAutoQuoteObject(int row);

public slots:
  void autoQuoteRunningChanged(int row);

private:
  QVector<AutoQuoteObjectPtr> auto_quote_stocks_;
};

IWKATS_NAMESPACE_END

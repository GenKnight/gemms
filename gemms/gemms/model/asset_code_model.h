#pragma once

#include "core/iwkats_core.h"

#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

class AssetCodeModel : public QAbstractListModel {
  Q_OBJECT
public:
  enum AlgoStockType {
    BeginUserRole = Qt::UserRole,
    KszStkCode,                         ///< 证券代码
    KszStkName,                         ///< 证券名称
    KchStkEx,                           ///< 证券交易所
    KszStkBd,                           ///< 证券板块
    KchCodeBizAction,                   ///< 股票类型
    KszCombiName,                       ///< 组合名称
    KszAssetName,                       ///< 资产单元名称
  };

  AssetCodeModel(QObject* parent = nullptr);

  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  Q_INVOKABLE QStringList customRoleNames();

private:
  struct Private;
  std::shared_ptr<Private> d_;
};

IWKATS_NAMESPACE_END

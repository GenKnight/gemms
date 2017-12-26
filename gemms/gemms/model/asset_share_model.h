#pragma once

#include "core/iwkats_core.h"
#include "core/util.hpp"
#include "KSMMApi/Define.h"

#include <QSet>
#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

// 查询资产
class AssetShareModel : public QAbstractListModel
{
  Q_OBJECT
public:
  enum eAssetShare {
    BeginUserRole = Qt::UserRole,
    KnProjectId,           // 项目ID
    KszProjectName,        // 项目名称
    KnAssetId,             // 资产单元ID
    KszAssetName,          // 资产单元名称
    KnCombiId,             // 组合ID
    KchStkex,              // 证券交易所
    KszStkbd,              // 交易板块
    KszStkCode,            // 证券代码
    KszStkName,            // 证券名称
    KchStkCls,             // 证券类别
    KllStkPrebln,          // 证券昨日余额
    KllStkBln,             // 证券余额
    KllStkAvl,             // 证券可用数量
    KdStkBcostRlt,         // 证券买入成本（实时）
    KdStkIncome,           // 今日实际盈亏金额
    KdStkIncomePre,        // 昨日实际盈亏金额
    KdStkFpl,              // 今日浮动盈亏金额
    KdMktVal,              // 市值
    EndUserRole
  };
public:
  AssetShareModel(QObject *parent = Q_NULLPTR);
  ~AssetShareModel() {}

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames()const override;
  Q_INVOKABLE QStringList customRoleNames();

  Q_INVOKABLE void refresh(const QString& stock);

public slots:
  void onAssetShareArrive(SharedVecPtr<ASSET_SHARE> pAssetShare);

private:
  QString stock_;
  QHash<QString, int> valid_checks_;
  QList<std::shared_ptr<ASSET_SHARE>> asset_shares_;
};

IWKATS_NAMESPACE_END

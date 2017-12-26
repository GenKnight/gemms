#pragma once

#include "core/iwkats_core.h"
#include "core/util.hpp"
#include "KSMMApi/Define.h"

#include <QAbstractListModel>

IWKATS_NAMESPACE_BEGIN

// 查询资产
class AssetFundModel : public QAbstractListModel
{
  Q_OBJECT
public:
  enum eAssetFund {
    BeginUserRole = Qt::UserRole,
    KnAssetId,         // 资产单元ID
    KszProjectName,    // 项目名称
    KdMaxMoney,        // 资金上限
    KdMinMoney,        // 资金下限
    KdFundPrebln,      // 资金昨日余额
    KdFundBln,         // 资金当前余额
    KdFundAvl,         // 资金可用金额
    KdFundFrz,         // 资金冻结金额
    KdFundUfz,         // 资金解冻金额
    KdInterest,        // 利息
    KdMktVal,          // 市值
    KdLastFundClr,     // 上次清算金额
    KchFundStatus,     // 资金状态
    KdPaylater,        // 垫付资金
    EndUserRole
  };
public:
  explicit AssetFundModel(QObject* parent = Q_NULLPTR);
  ~AssetFundModel() {}
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames()const override;
  Q_INVOKABLE QStringList customRoleNames();

  Q_INVOKABLE void refresh();

public slots:
  void onAssetFundArrive(SharedVecPtr<ASSET_FUND> pAssetFund);

private:
  QHash<int, int> valid_checks_;
  QList<std::shared_ptr<ASSET_FUND>> asset_funds_;
};

IWKATS_NAMESPACE_END


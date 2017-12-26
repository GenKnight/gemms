#include "asset_code_model.h"

#include <vector>

#include "core/logging.h"

#include "ksmm/ksmm_trader.h"
#include "ksmm/common_define.hpp"
#include "ksmm/stock_object.h"

IWKATS_NAMESPACE_BEGIN

struct AssetCodeModel::Private {
  std::vector<ASSET_CODE*> asset_codes;
};

AssetCodeModel::AssetCodeModel(QObject *parent)
  : QAbstractListModel(parent)
  , d_(new Private) {

  beginResetModel();
  Trader::Spi()->GetAssetCodes(d_->asset_codes);
  endResetModel();
}

int AssetCodeModel::rowCount(const QModelIndex& parent) const
{
  return d_->asset_codes.size();
}

QVariant AssetCodeModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() )
    return QVariant();

  if( index.row() >= rowCount()) {
    LOG_ERROR << "AutoQuoteStockListModel: something wrong here...";
    return QVariant();
  }

  ASSET_CODE* asset_code = d_->asset_codes[index.row()];
  if( role == Qt::DisplayRole ) {
    QString stock_info("");
    if(asset_code) {
      stock_info += core_util::toUtf8(asset_code->szStkName);
      stock_info += " ";
      stock_info += asset_code->szStkCode;
    }
    return stock_info;
  }

  switch(role) {
  case KszStkCode:            return asset_code->szStkCode;
  case KszStkName:            return QString::fromLocal8Bit(asset_code->szStkName);
  case KchStkEx:              return GetStkExMean(asset_code->chStkEx);
  case KszStkBd:              return GetSzStkBdMean(asset_code->szStkBd);
  case KchCodeBizAction:      return GetCodeBizActionMean(asset_code->chCodeBizAction);
  case KszCombiName:          return QString::fromLocal8Bit(asset_code->szCombiName);
  case KszAssetName:          return QString::fromLocal8Bit(asset_code->szAssetName);
  default: return QVariant();
  }
  return QVariant();
}

QHash<int, QByteArray> AssetCodeModel::roleNames() const
{
  QHash<int, QByteArray> roles_names;
  roles_names[KszStkCode] = core_util::toUtf8("证券代码");
  roles_names[KszStkName] = core_util::toUtf8("证券名称");
  roles_names[KchStkEx] = core_util::toUtf8("证券交易所");
  roles_names[KszStkBd] = core_util::toUtf8("证券板块");
  roles_names[KchCodeBizAction] = core_util::toUtf8("股票类型");
  roles_names[KszCombiName] = core_util::toUtf8("组合名称");
  roles_names[KszAssetName] = core_util::toUtf8("资产单元名称");
  return roles_names;
}

QStringList AssetCodeModel::customRoleNames()
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

IWKATS_NAMESPACE_END

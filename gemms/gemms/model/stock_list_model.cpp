#include "stock_list_model.h"

#include <QSet>
#include <QReadWriteLock>

#include "core/lock.h"
#include "ksmm/stock_object.h"

#include "ksmm/ksmm_trader.h"

IWKATS_NAMESPACE_BEGIN

StockListModel::StockListModel(QObject *parent)
  : QAbstractListModel(parent) {
  std::vector<ASSET_CODE*> codes;
  Trader::Spi()->GetAssetCodes(codes);
  for(auto asset_code : codes) {

    StockObjectPtr stock_ptr = std::make_shared<StockObject>();
    stock_ptr->setStock(asset_code->szStkCode);
    stock_ptr->setType(StockObject::QUOTE_BY_HAND);

    model_stocks_.insert(asset_code->szStkCode, stock_ptr);
    total_stocks_.insert(asset_code->szStkCode, stock_ptr);
  }
}

int StockListModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent);
  return model_stocks_.size();
}

QVariant StockListModel::data(const QModelIndex& index, int role) const {
  int row = index.row();
  if(row < 0 || row >= rowCount()) {
    return QVariant();
  }

  ASSET_CODE* asset_code{nullptr};
  StockObjectPtr stock_obj_ptr = GetStockObjectByRow(row);
  if(stock_obj_ptr) {
    asset_code = stock_obj_ptr->assetCode();
  }
  if(nullptr == asset_code) {
    return QVariant();
  }

  switch(role) {
    case KszStkCode:            return asset_code->szStkCode;
    case KszStkName:            return QString::fromLocal8Bit(asset_code->szStkName);
    case KchStkEx:              return GetOrderStatusMean(asset_code->chStkEx);
    case KszStkBd:              return GetSzStkBdMean(asset_code->szStkBd);
    case KchCodeBizAction:      return GetCodeBizActionMean(asset_code->chCodeBizAction);
    case KszCombiName:          return asset_code->szCombiName;
    case KszAssetName:          return QString::fromLocal8Bit(asset_code->szAssetName);
    default: return QVariant();
  }
  return QVariant();
}

QHash<int, QByteArray> StockListModel::roleNames() const {
  QHash<int, QByteArray> roles_names;
  roles_names[KszStkCode] = "szStkCode";
  roles_names[KszStkName] = "szStkName";
  roles_names[KchStkEx] = "chStkEx";
  roles_names[KszStkBd] = "szStkBd";
  roles_names[KchCodeBizAction] = "chCodeBizAction";
  roles_names[KszCombiName] = "szCombiName";
  roles_names[KszAssetName] = "szAssetName";
  return roles_names;
}

StockObject* StockListModel::getStockObjectByRow(int row) {
  if(row < 0 || row >= rowCount()) {
    return nullptr;
  }
  StockObjectPtr stock_obj_ptr = GetStockObjectByRow(row);
  if(stock_obj_ptr) {
    return stock_obj_ptr.get();
  }
  return nullptr;
}

StockObject* StockListModel::getStockObjectByStock(const QString& stock) {
  StockObjectPtr stock_obj_ptr = nullptr;
  QHashIterator<QString, StockObjectPtr> i(model_stocks_);
  while(i.hasNext()) {
    i.next();
    const auto& curr_stock_obj_ptr = i.value();
    if(curr_stock_obj_ptr->stock() == stock) {
      stock_obj_ptr = curr_stock_obj_ptr;
      break;
    }
  }
  if(stock_obj_ptr) {
    return stock_obj_ptr.get();
  }
  return nullptr;
}

void StockListModel::remove(const QString& stock) {
  QWriteLocker wlock(&rwlock_);
  beginResetModel();
  model_stocks_.remove(stock);
  endResetModel();
}

void StockListModel::add(const QString& stock) {
  QWriteLocker wlock(&rwlock_);
  const auto& stock_obj_ptr = total_stocks_.value(stock, nullptr);
  if(stock_obj_ptr) {
    beginResetModel();
    model_stocks_.insert(stock, stock_obj_ptr);
    endResetModel();
  }
}

QString StockListModel::getStockName(const QString& stock) {
  QReadLocker rlock(&rwlock_);
  StockObjectPtr stock_obj_ptr = total_stocks_.value(stock, nullptr);
  if(stock_obj_ptr) {
    return stock_obj_ptr->stockName();
  }
  return "";
}

StockObjectPtr StockListModel::GetStockObjectByRow(int row) const {
  int index = -1;
  StockObjectPtr stock_obj_ptr = nullptr;
  QHashIterator<QString, StockObjectPtr> i(model_stocks_);
  while(i.hasNext()) {
    i.next();
    index += 1;
    if(row == index) {
      stock_obj_ptr = i.value();
      break;
    }
  }
  return stock_obj_ptr;
}


IWKATS_NAMESPACE_END

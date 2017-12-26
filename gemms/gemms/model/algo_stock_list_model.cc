#include "algo_stock_list_model.h"

#include "core/logging.h"

#include "ksmm/ksmm_trader.h"
#include "ksmm/auto_quote/auto_quote_object.h"

IWKATS_NAMESPACE_BEGIN

AlgoStockListModel::AlgoStockListModel(QObject *parent)
  : QAbstractListModel(parent) {

  beginResetModel();

  std::vector<ASSET_CODE*> codes;
  Trader::Spi()->GetAssetCodes(codes);
  int row = -1;
  for(auto& asset_code : codes) {
    AutoQuoteObjectPtr auto_quote_ptr = std::make_shared<AutoQuoteObject>();
    auto_quote_ptr->setStock(asset_code->szStkCode);
    auto_quote_ptr->setRow(++row);
    connect(auto_quote_ptr.get(), &AutoQuoteObject::runningChanged, this, &AlgoStockListModel::autoQuoteRunningChanged);
    auto_quote_stocks_.push_back(auto_quote_ptr);
  }

  endResetModel();
}

int AlgoStockListModel::rowCount(const QModelIndex& parent) const
{
  return auto_quote_stocks_.size();
}

QVariant AlgoStockListModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
    return QVariant();

  auto row = index.row();
  if( row < 0 || row >= rowCount()) {
    LOG_ERROR << "AutoQuoteStockListModel: something wrong here...";
    return QVariant();
  }

  AutoQuoteObjectPtr auto_quote_ptr = auto_quote_stocks_[row];
  ASSET_CODE* asset_code = Trader::Spi()->GetAssetCode(auto_quote_ptr->stock());
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
  case KszStkCode:            
    return asset_code->szStkCode;
  case KszStkName:            return QString::fromLocal8Bit(asset_code->szStkName);
  case KchStkEx:              return asset_code->chStkEx;
  case KszStkBd:              return asset_code->szStkBd;
  case KchCodeBizAction:      return asset_code->chCodeBizAction;
  case KszCombiName:          return asset_code->szCombiName;
  case KszAssetName:          return asset_code->szAssetName;
  case KisRunning:            return auto_quote_ptr->isRunning();
  default: return QVariant();
  }
  return QVariant();
}

QHash<int, QByteArray> AlgoStockListModel::roleNames() const
{
  QHash<int, QByteArray> roles_names = QAbstractListModel::roleNames();
  roles_names[KszStkCode] = "szStkCode";
  roles_names[KszStkName] = "szStkName";
  roles_names[KchStkEx] = "chStkEx";
  roles_names[KszStkBd] = "szStkBd";
  roles_names[KchCodeBizAction] = "chCodeBizAction";
  roles_names[KszCombiName] = "szCombiName";
  roles_names[KszAssetName] = "szAssetName";
  roles_names[KisRunning] = "isRunning";
  return roles_names;
}

AutoQuoteObject* AlgoStockListModel::getAutoQuoteObject(int row)
{
  if(row < 0 || row >= rowCount()) {
    return nullptr;
  }
  return auto_quote_stocks_[row].get();
}

void AlgoStockListModel::autoQuoteRunningChanged(int row)
{
  if(row < 0 || row >= rowCount()) {
    return;
  }
  emit dataChanged(index(row), index(row));
}

IWKATS_NAMESPACE_END

#include "market_book_model.h"

#include <array>

#include "core/logging.h"

#include "ksmm/ksmm_trader.h"
#include "ksmm/market/market_manager.h"
#include "ksmm/common_define.hpp"

IWKATS_NAMESPACE_BEGIN

struct MarketBookModel::Private {
  std::array<Tick, MARKET_BOOK_MAX_SIZE> zs_book{0};
  std::array<Tick, MARKET_BOOK_MAX_SIZE> tz_book{0};
};

MarketBookModel::MarketBookModel(QObject* parent)
  : QAbstractListModel(parent)
  , d_(new Private)

{
  connect(MarketManager::GetPtr(), &MarketManager::sigNqTickArrive, this, &MarketBookModel::slotNqTickArrive);
  connect(MarketManager::GetPtr(), &MarketManager::sigZsTickArrive, this, &MarketBookModel::slotZsTickArrive);
}

void MarketBookModel::setStock(const QString& stock)
{
  if(code_ != stock) {
    code_ = stock;
    resetModel();
  }
}

int MarketBookModel::rowCount(const QModelIndex& parent) const
{
  if(ZSMarketType == type_ || TZMarketType == type_) {
    return MARKET_BOOK_MAX_SIZE;
  }
  return 0;
}

QVariant MarketBookModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()) {
    return 0;
  }

  auto row = index.row();
  if(row < 0 || row >= MARKET_BOOK_MAX_SIZE) {
    return 0;
  }

  Tick& tick = (ZSMarketType == type_ ? d_->zs_book.at(row) : d_->tz_book.at(row));
  switch (role) {
    case Price:
      return QString::number(tick.price, 'f', 2);
      break;
    case Volume:
      return tick.vol;
      break;
    default:
      break;
  }
  return 0;
}

QHash<int, QByteArray> MarketBookModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[Price] = "Price";
  roles[Volume] = "Volume";
  return roles;
}

void MarketBookModel::resetModel()
{
  // 获取当前最新行情
  const auto& pNQHQInfo = MarketManager::GetPtr()->GetStockNQTick(code_);
  if(pNQHQInfo) {
    slotNqTickArrive(pNQHQInfo);
  }

  const auto& pZSHQInfo = MarketManager::GetPtr()->GetStockZSTick(code_);
  if(pZSHQInfo) {
    slotZsTickArrive(pZSHQInfo);
  }
}

void MarketBookModel::slotNqTickArrive(std::shared_ptr<STK_NQ_TICK> pNQHQInfo)
{
  // 只更新当前股票的
  if(0 != strcmp(code_.toStdString().data(), pNQHQInfo->szStkCode)) {
    return;
  }
  //  LOG_INFO << pNQHQInfo->szStkName << " " << pNQHQInfo->szStkCode << " dBidPrice1: " << pNQHQInfo->dNQDishPri[0][0];
}

void MarketBookModel::slotZsTickArrive(std::shared_ptr<STK_NQ_ZSXX> pZSHQInfo)
{
  if(ZSMarketType != type_
     && TZMarketType != type_) {
    return;
  }

  // 只更新当前股票的
  if(0 != strcmp(code_.toStdString().data(), pZSHQInfo->szStkCode)) {
    return;
  }

  beginResetModel();

  // 清空当前的book
  if(ZSMarketType == type_) {
    clearAllTickBook(d_->zs_book);
  } else if(TZMarketType == type_) {
    clearAllTickBook(d_->tz_book);
  }

  for(auto i = 0; i < 2; ++i) {
    // 买10档价格和数量
    if(0 == i) {
      const auto& price_array = (ZSMarketType == type_ ? pZSHQInfo->dMMDishPri[0] : pZSHQInfo->dMDDishPri[0]);
      const auto& vol_array = (ZSMarketType == type_ ? pZSHQInfo->nMMDishVol[0] : pZSHQInfo->nMDDishVol[0]);

      // 买1 - 买10
      for(auto j = 0; j < 10; ++j) {
        auto dAskPrice = price_array[j];
        auto nAskVol = vol_array[j];

        auto tick_index = std::abs(j + 11);
        Tick& tick = (ZSMarketType == type_ ? d_->zs_book.at(tick_index) : d_->tz_book.at(tick_index));
        tick.price = dAskPrice;
        tick.vol = nAskVol;
      }
    }

    // 卖10档价格和数量
    if(1 == i) {
      const auto& price_array = (ZSMarketType == type_ ?  pZSHQInfo->dMMDishPri[1] : pZSHQInfo->dMDDishPri[1]);
      const auto& vol_array = (ZSMarketType == type_ ? pZSHQInfo->nMMDishVol[1] : pZSHQInfo->nMDDishVol[1]);
      // 卖1 - 卖10
      auto currAskIndex = 9;
      for(auto j = 9; j >= 0; --j) {
        // 找到当前不为0的行情
        auto dAskPrice = price_array[j];
        auto nAskVol = vol_array[j];
        if(0.0 == dAskPrice && 0 == nAskVol) {
          continue;
        }
        // 顺序更新到 currAskIndex 处
        auto tick_index = currAskIndex--;
        Tick& tick = (ZSMarketType == type_ ? d_->zs_book.at(tick_index) : d_->tz_book.at(tick_index));
        tick.price = dAskPrice;
        tick.vol = nAskVol;
      }
    }
  }

  endResetModel();
}

void MarketBookModel::clearAllTickBook(std::array<Tick, MARKET_BOOK_MAX_SIZE>& book_tick)
{
  for(auto i = 0; i < MARKET_BOOK_MAX_SIZE; ++i){
    auto& tick = book_tick[i];
    tick.price = 0;
    tick.vol = 0;
  }
}

IWKATS_NAMESPACE_END

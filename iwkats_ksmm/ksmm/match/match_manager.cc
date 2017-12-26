#include "match_manager.h"

#include "core/ttspeech.h"

#include "ksmm/ksmm_trader.h"
#include "ksmm/common_define.hpp"

IWKATS_NAMESPACE_BEGIN

struct MatchManager::Private {
  QReadWriteLock rwlock;
  QHash<QString, MatchInfoPtr> all_matchs;
  QHash<QString, QHash<QString, MatchInfoPtr>> all_stocks_matchs;
};

//////////////////////////////////////////////////////////////////////////

MatchManager::MatchManager(QObject *parent)
  : QObject(parent)
  , d_(new Private) {
  connect(Trader::Spi(), &KSMMTradingSpi::sigMatchArrive, this, &MatchManager::slotMatchArrive);
}

MatchManager* MatchManager::GetPtr() {
  static MatchManager instance;
  return &instance;
}

bool MatchManager::GetAllMatchByStock(const QString& stock, QList<MatchInfoPtr>& all_matchs)
{
  RLockGuard rlock(d_->rwlock);
  if(d_->all_stocks_matchs.contains(stock)) {
    all_matchs = d_->all_stocks_matchs.value(stock).values();
    return true;
  }
  return false;
}

bool MatchManager::GetMatchByOrderId(const QString& order_id, MatchInfoPtr& match_ptr)
{
  RLockGuard rlock(d_->rwlock);
  if(d_->all_matchs.contains(order_id)) {
    match_ptr = d_->all_matchs.value(order_id);
    return true;
  }
  return false;
}

void MatchManager::GetAllMatchs(QList<MatchInfoPtr>& all_matchs)
{
  all_matchs = d_->all_matchs.values();
}

// 成交回报
void MatchManager::slotMatchArrive(std::shared_ptr<MATCH_INFO> pMatchInfo)
{
  WLockGuard wlock(d_->rwlock);
  MatchInfoPtr match_ptr = std::make_shared<MatchInfo>(pMatchInfo);
  d_->all_matchs[pMatchInfo->szOrderID] = match_ptr;
  d_->all_stocks_matchs[pMatchInfo->szStkCode].insert(pMatchInfo->szOrderID, match_ptr);
  emit sigMatchArrive(match_ptr);
}

IWKATS_NAMESPACE_END

#pragma once

#include "core/iwkats_core.h"

#include <QObject>

#include "KSMMApi/Define.h"

IWKATS_NAMESPACE_BEGIN

class IWKATS_CORE_EXPORT MatchInfo : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString szOrderID READ szOrderID)
  Q_PROPERTY(QString szStkCode READ szStkCode)
  Q_PROPERTY(double dAskOrderPrice READ dAskOrderPrice)
  Q_PROPERTY(double dBidOrderPrice READ dBidOrderPrice)
  Q_PROPERTY(int nAskOrderQty READ nAskOrderQty)
  Q_PROPERTY(int nBidOrderQty READ nBidOrderQty)
  Q_PROPERTY(QString chIsWithdraw READ chIsWithdraw)

public:
  explicit MatchInfo(std::shared_ptr<MATCH_INFO> match_ptr);
  std::shared_ptr<MATCH_INFO> match_ptr() const { return match_ptr_; }

  QString stock() const { return match_ptr_->szStkCode; }
  QString matchDetails() const { return match_details_; }
  QString infoBarMsg() const { return info_msg_; }
  QString speechMsg() const { return speech_msg_; }

  QString szOrderID() const { return match_ptr_->szOrderID; }
  QString szStkCode() const { return match_ptr_->szStkCode; }
  double dAskOrderPrice() const { return match_ptr_->dAskOrderPrice; }
  double dBidOrderPrice() const { return match_ptr_->dBidOrderPrice; }
  int nAskOrderQty() const { return match_ptr_->nAskMatchedQty; }
  int nBidOrderQty() const { return match_ptr_->nBidOrderQty;}
  QString chIsWithdraw() const { return match_ptr_->chIsWithdraw;}

  std::shared_ptr<MATCH_INFO> GetMatchInfoPtr() const { return match_ptr_; }

private:
  QString match_details_{""};
  QString info_msg_{""};
  QString speech_msg_{""};
  std::shared_ptr<MATCH_INFO> match_ptr_{nullptr};
};

using MatchInfoPtr = std::shared_ptr<MatchInfo>;

IWKATS_NAMESPACE_END

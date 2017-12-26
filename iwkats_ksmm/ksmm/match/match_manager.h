#pragma once

#include "core/iwkats_core.h"
#include "core/lock.h"

#include <QHash>
#include <QObject>
#include <QReadWriteLock>

#include "match_info.h"

IWKATS_NAMESPACE_BEGIN

// 成交回报
class IWKATS_CORE_EXPORT MatchManager : public QObject {
  Q_OBJECT
  MatchManager(QObject *parent = nullptr);
public:
  virtual ~MatchManager() = default;
  static MatchManager* GetPtr();

  bool GetAllMatchByStock(const QString& stock, QList<MatchInfoPtr>& all_matchs);
  bool GetMatchByOrderId(const QString& order_id, MatchInfoPtr& match_ptr);
  void GetAllMatchs(QList<MatchInfoPtr>& all_matchs);

signals:
  void sigMatchArrive(MatchInfoPtr match_ptr);

public slots:
  void slotMatchArrive(std::shared_ptr<MATCH_INFO> pMatchInfo);

private:
  struct Private;
  std::shared_ptr<Private> d_;
};


IWKATS_NAMESPACE_END

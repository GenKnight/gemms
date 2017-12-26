#pragma once

#include "core/iwkats_core.h"

#include <QObject>

#include "ksmm/ksmm_trader.h"
#include "order_info_model.h"

IWKATS_NAMESPACE_BEGIN

class InfoBarModel : public QObject {
  Q_OBJECT
  Q_ENUMS(InfoNoticeType)
public:
  enum InfoNoticeType {
    ErrorNotice,
    OrderNotice,

  };

  InfoBarModel(QObject* parent = nullptr);

public slots:
  void slotNotice(std::shared_ptr<LBM_ERROR_INFO> pRspError);

signals:
  void sigNotice(const QString& code, int type, const QString& msg);
};

IWKATS_NAMESPACE_END

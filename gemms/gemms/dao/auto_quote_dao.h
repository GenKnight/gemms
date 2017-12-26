#pragma once

#include "ksmm/common_define.hpp"
#include "core/iwkats_core.h"

#include <QObject>
#include <QSqlDatabase>

IWKATS_NAMESPACE_BEGIN

class AutoQuoteDao : public QObject {
  Q_OBJECT
    AutoQuoteDao();
public:
  static AutoQuoteDao* GetInstance();

private:
  void ConnectDatabase();
  // 建表
  void CreateAutoQuoteTable();

public:


private:
  QSqlDatabase auto_quote_db_;
};

IWKATS_NAMESPACE_END

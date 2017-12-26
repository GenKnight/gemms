#pragma once

#include "core/iwkats_core.h"
#include "ksmm/common_define.hpp"

#include <QObject>
#include <QTimer>
#include <QThread>

IWKATS_NAMESPACE_BEGIN

class AutoQuoteThread {
  AutoQuoteThread() = default;
public:
  static AutoQuoteThread* GetPtr();
  ~AutoQuoteThread();

  QThread* thread();

private:
  QThread thread_;
};

//////////////////////////////////////////////////////////////////////////

class StockObject;
class AutoQuoteWorker;
class IWKATS_CORE_EXPORT AutoQuoteObject : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString stock READ stock WRITE setStock)
  Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningChanged)
  Q_PROPERTY(StockObject* stockObject READ stockObject NOTIFY stockObjectChanged)

public:
  AutoQuoteObject(QObject* parent = nullptr);

  // getter
  QString stock() const;
  bool isRunning() const;
  StockObject* stockObject() const;

  // setter
  void setStock(const QString& stock);
  void setRow(int row);

  // 开始自动化做市
  Q_INVOKABLE bool startAutoQuote();

  // 停止自动化做市
  Q_INVOKABLE bool stopAutoQuote();

public slots:
  // 启动停止
  void slotAutoQuoteStarted();
  void slotAutoQuoteStoped();
  void AutoQuoteLogStdMsg(const std::string& msg);
  // 添加日志
  void AutoQuoteLogMsg(QString msg);
  // 发生错误
  void slotStockQuoteError(QString error);

signals:
  // 启动
  void sigStartAutoQuote();
  // 停止
  void sigStopStartAutoQuote();
  // 运行状态改变
  void runningChanged(int row);

  void stockObjectChanged();

  // 委托
  void sigOrderInfoChanged(OrderInfo* orderInfo);
  void sigAppendLog(QString msg);
  void sigAutoQuoteError(QString error);

private:
  struct Private;
  std::shared_ptr<Private> d_;
};

IWKATS_NAMESPACE_END

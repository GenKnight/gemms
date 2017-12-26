#include "auto_quote_object.h"

#include <QVariant>
#include <QThread>
#include <QMessageBox>

#include "core/util.hpp"

#include "ksmm/ksmm_trader.h"
#include "ksmm/order/order_info.h"
#include "ksmm/stock_object.h"
#include "ksmm/market/market_manager.h"

#include "auto_quote_worker.h"

IWKATS_NAMESPACE_BEGIN

AutoQuoteThread::~AutoQuoteThread() {
  thread_.quit();
  thread_.wait();
}

AutoQuoteThread* AutoQuoteThread::GetPtr() {
  static AutoQuoteThread instance;
  return &instance;
}

QThread* AutoQuoteThread::thread() {
  return &thread_;
}

//////////////////////////////////////////////////////////////////////////

struct AutoQuoteObject::Private {
  QString stock{""};
  int row{-1};
  std::shared_ptr<AutoQuoteWorker> worker;
};

//////////////////////////////////////////////////////////////////////////

AutoQuoteObject::AutoQuoteObject(QObject *parent)
  : QObject(parent)
  , d_(new Private) {

}

QString AutoQuoteObject::stock() const {
  return d_->stock;
}

bool AutoQuoteObject::isRunning() const {
  return d_->worker->isRunning();
}

StockObject*AutoQuoteObject::stockObject() const {
  return d_->worker->stockObject();
}

void AutoQuoteObject::setStock(const QString& stock) {
  if(stock != d_->stock) {
    d_->stock = stock;
    d_->worker = std::make_shared<AutoQuoteWorker>(stock);

    // 启动
    connect(this, &AutoQuoteObject::sigStartAutoQuote, d_->worker.get(), &AutoQuoteWorker::startAutoQuote);
    connect(d_->worker.get(), &AutoQuoteWorker::sigAutoQuoteStarted, this, &AutoQuoteObject::slotAutoQuoteStarted);

    // 停止
    connect(this, &AutoQuoteObject::sigStopStartAutoQuote, d_->worker.get(), &AutoQuoteWorker::stopAutoQuote);
    connect(d_->worker.get(), &AutoQuoteWorker::sigAutoQuoteStoped, this, &AutoQuoteObject::slotAutoQuoteStoped);

    // 日志
    connect(d_->worker.get(), &AutoQuoteWorker::sigAutoQuoteLog, this, &AutoQuoteObject::AutoQuoteLogMsg);
    // 错误
    connect(d_->worker.get(), &AutoQuoteWorker::sigStockQuoteError, this, &AutoQuoteObject::slotStockQuoteError);

    // 委托发生变化
    connect(d_->worker.get(), &AutoQuoteWorker::sigOrderInfoChanged, this, &AutoQuoteObject::sigOrderInfoChanged);
    d_->worker->moveToThread(AutoQuoteThread::GetPtr()->thread());

    // 开启自动重报价线程
    if(!AutoQuoteThread::GetPtr()->thread()->isRunning()) {
      AutoQuoteThread::GetPtr()->thread()->start();
    }
  }
}

void AutoQuoteObject::setRow(int row) {
  d_->row = row;
}

bool AutoQuoteObject::startAutoQuote() {
  // 检查是否已经在运行
  if(isRunning()) {
    AutoQuoteLogStdMsg("股票[" + stock().toStdString() + "]自动重复报价策略已经在运行");
    return true;
  }

  if(stock().isEmpty()) {
    AutoQuoteLogStdMsg("股票[" + stock().toStdString() + "]股票代码非法!");
    return false;
  }

  // 开始运行
  emit sigStartAutoQuote();
  return true;
}

bool AutoQuoteObject::stopAutoQuote() {
  // 检查是否已经停止运行
  if(!isRunning()) {
    return true;
  }
  emit sigStopStartAutoQuote();
  return true;
}

void AutoQuoteObject::slotAutoQuoteStarted() {
  if(isRunning()) {
    AutoQuoteLogStdMsg("股票[" + stock().toStdString() + "]自动重复报价策略 启动成功");
    AutoQuoteLogStdMsg("股票[" + stock().toStdString() + "]订阅行情成功");
    emit runningChanged(d_->row);
  }
}

void AutoQuoteObject::slotAutoQuoteStoped() {
  if(!isRunning()) {
    emit runningChanged(d_->row);
    AutoQuoteLogStdMsg("股票[" + stock().toStdString() + "]自动重复报价策略已停止运行");
  }
}

void AutoQuoteObject::AutoQuoteLogStdMsg(const string& msg) {
  AutoQuoteLogMsg(QString::fromLocal8Bit(msg.data()));
}

void AutoQuoteObject::AutoQuoteLogMsg(QString msg) {
  // 头部
  std::ostringstream oss;
  oss << currentTimePointString() << ": ";
  QString str_msg_head = QString::fromLocal8Bit(oss.str().data());
  emit sigAppendLog(str_msg_head + msg);
}

void AutoQuoteObject::slotStockQuoteError(QString error) {
  AutoQuoteLogMsg(error);

  stopAutoQuote();

  emit runningChanged(d_->row);
  emit sigAutoQuoteError(error);
}

IWKATS_NAMESPACE_END

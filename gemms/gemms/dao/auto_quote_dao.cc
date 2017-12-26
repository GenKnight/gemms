#include "auto_quote_dao.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QVariant>
#include <qapplication.h>

#include "core/util.hpp"

IWKATS_NAMESPACE_BEGIN

static const QString kAutoQuoteDatabaseConnection = "gemms_auto_quote_connection";

AutoQuoteDao::AutoQuoteDao() {
  ConnectDatabase();
  CreateAutoQuoteTable();
}

AutoQuoteDao*AutoQuoteDao::GetInstance() {
  static AutoQuoteDao instance;
  return &instance;
}

// 连接数据库
void AutoQuoteDao::ConnectDatabase() {
  auto_quote_db_ = QSqlDatabase::addDatabase("QSQLITE", kAutoQuoteDatabaseConnection);
  auto_quote_db_.setDatabaseName("AutoQuote.db");
  auto_quote_db_.open();


  if(!QFile::exists("./AutoQuote.db")) {
    QString failedMsg = ("FAILED: Could not locate AuoteQuote.db file");
    QMessageBox::warning(nullptr, "ERROR: Failed AutoQuote Database Connection!", failedMsg);
    qApp->exit(0);
  }

  if(!auto_quote_db_.isOpen()) {
    QString failedMsg = QObject::tr("FAILED: ") + auto_quote_db_.lastError().text();
    QMessageBox::warning(nullptr, QObject::tr("ERROR: Failed to open AutoQuote.db!"), failedMsg);
    qApp->exit(0);
  }
}

// 建立自动重复报价策略参数配置数据库表
void AutoQuoteDao::CreateAutoQuoteTable() {
  QSqlQuery create_table_sql(auto_quote_db_);
  create_table_sql.prepare("CREATE TABLE IF NOT EXISTS gemms_auto_quote(stock VARCHAR(6) PRIMARY KEY NOT NULL,auto_quote_info TEXT);");
  create_table_sql.exec();
}

//// 增加自动重复报价策略参数配置记录
//bool AutoQuoteDao::AddAutoQuoteInfo(QString stock, const AutoQuoteInfo* auto_quote_info_ptr) {
//  std::string result;
//  if(!toJson(*auto_quote_info_ptr, result)) {
//    return false;
//  }
//  LOG_DEBUG << result;
//  QSqlQuery update(auto_quote_db_);
//  update.prepare("REPLACE INTO main.gemms_auto_quote(stock, auto_quote_info) VALUES (:stock, :auto_quote_info)");
//  update.bindValue(":stock", stock);
//  update.bindValue(":auto_quote_info", QString::fromLocal8Bit(result.data()));
//  update.exec();
//  return true;
//}

//// 查询自动重复报价策略参数配置缓存
//AutoQuoteInfoPtr AutoQuoteDao::QueryAutoQuoteInfo(QString stock) {
//  auto asset_code = Trader::Spi()->GetAssetCode(stock);
//  if(!asset_code) {
//    return nullptr;
//  }

//  // 读取数据库
//  QSqlQuery query_sql(auto_quote_db_);
//  query_sql.prepare("select auto_quote_info from main.gemms_auto_quote where stock = :stock");
//  query_sql.bindValue(":stock", stock);
//  query_sql.exec();

//  if(query_sql.next()) {
//    AutoQuoteInfoPtr p_query_result = std::make_shared<AutoQuoteInfo>(asset_code);
//    fromJson(*p_query_result, query_sql.value("auto_quote_info").toString().toStdString());
//    return p_query_result;
//  }
//  return nullptr;
//}

IWKATS_NAMESPACE_END

#pragma once

#include "core/iwkats_core.h"

#include <QObject>

IWKATS_NAMESPACE_BEGIN

class AutoQuoteInfo;

class UserApi : public QObject
{
  Q_OBJECT
public:
  explicit UserApi(QObject *parent = nullptr);

  // 获取当前错误
  Q_INVOKABLE QString getLastError();

  // 请求登陆
  Q_INVOKABLE void userReqLogin(const QString& user_name, const QString& password);


signals:
  void loginResult(bool result);
  void loginStatusChanged(QString msg);
  void autoQuoteNotice(const QString& msg);

};

IWKATS_NAMESPACE_END


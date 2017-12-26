#include "user_api.h"

#include "ksmm/ksmm_trader.h"

IWKATS_NAMESPACE_BEGIN

UserApi::UserApi(QObject *parent) : QObject(parent)
{
  connect(Trader::Spi(), &KSMMTradingSpi::loginStatusChanged, this, &UserApi::loginStatusChanged);
}

QString UserApi::getLastError()
{
  return Trader::Spi()->getLastError();
}

void UserApi::userReqLogin(const QString& user_name, const QString& password) {
  Trader::Api()->requestLogin(user_name, password);
}

IWKATS_NAMESPACE_END

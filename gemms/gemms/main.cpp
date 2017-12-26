
#include "core/settings.h"
#include "core/logging.h"
#include "core/screen_info.h"
#include "core/util.hpp"
#include "core/ttspeech.h"

#include "ksmm/common_define.hpp"
#include "ksmm/ksmm_trader.h"
#include "ksmm/ksmm_trading_api.h"

#include "ksmm/stock_object.h"
#include "ksmm/order/order_info.h"
#include "ksmm/match/match_info.h"

#include "ksmm/auto_quote/auto_quote_object.h"

#include "user_api.h"

#include "model/asset_code_model.h"
#include "model/market_book_model.h"
#include "model/stock_list_model.h"
#include "model/order_info_model.h"
#include "model/match_info_model.h"
#include "model/asset_fund_model.h"
#include "model/asset_share_model.h"
#include "model/quote_order_info_model.h"
#include "model/quote_match_info_model.h"
#include "model/infobar_model.h"

#include "model/algo_stock_list_model.h"

#include <QObject>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QApplication>
#include <qt_windows.h>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QApplication>
#include <QDesktopWidget>
#include <QQuickWindow>
#include <QProcess>
#include <QThreadPool>
#include <QMessageBox>
#include <QLocalSocket>
#include <QLocalServer>
#include <QAbstractSocket>

#include "quickflux/qfappdispatcher.h"

#define QML_REGISTER(a) qmlRegisterType<a>("qiwookong", 1, 0, #a)
#define QML_REGISTER_SINGLETON(a,b) qmlRegisterSingletonType<a>("qiwookong", 1, 0, #a, b);

using namespace iwkats;

QLocalServer *kLocalServer{nullptr};

Q_DECLARE_METATYPE(MatchInfo*)
Q_DECLARE_METATYPE(OrderInfo*)

int main(int argc, char** argv) {
  QApplication a(argc, argv);
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  a.setQuitOnLastWindowClosed(true);
  QApplication::setApplicationName("gemms");
  QApplication::setOrganizationName("221 Data Service");
  QApplication::setOrganizationDomain("http://www.221data.com/");
  QApplication::setApplicationVersion("1.0");

#if defined(Q_OS_OSX)
  QApplication::setQuitOnLastWindowClosed(false);
#else
  QApplication::setQuitOnLastWindowClosed(true);
#endif

  QQuickStyle::setStyle(QLatin1String("Material"));

  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::GLOG_INFO, "./Log/INFO_");
  google::SetLogDestination(google::GLOG_WARNING, "./Log/WARNING_");
  google::SetLogDestination(google::GLOG_ERROR, "./Log/ERROR_");
  FLAGS_colorlogtostderr = true;
  FLAGS_alsologtostderr = true;

  // 检查唯一性
  QString serverName = QCoreApplication::applicationName()
    + QString::fromLatin1(QT_VERSION_STR).remove('.') + QLatin1String("-221DataService");
  QLocalSocket socket;
  socket.connectToServer(serverName);
  if(socket.waitForConnected(500)) {
    auto ret = QMessageBox::warning(nullptr, "Gemms", core_util::toUtf8("您已经打开了自动化做市交易系统"), QMessageBox::Ok);
    qApp->quit();
    return 0;
  }

  kLocalServer = new QLocalServer();
  if(!kLocalServer->listen(serverName)) {
    if(kLocalServer->serverError() == QAbstractSocket::AddressInUseError) {
      QLocalServer::removeServer(serverName);
      if(!kLocalServer->listen(serverName))
        qWarning("Could not create local socket %s.", qPrintable(serverName));
    }
  }

  LOG_INFO << "QThreadPool maxThreadCount: " << QThreadPool::globalInstance()->maxThreadCount();

  qRegisterMetaType<std::shared_ptr<STK_NQ_TICK>>("std::shared_ptr<STK_NQ_TICK>");
  qRegisterMetaType<std::shared_ptr<STK_NQ_ZSXX>>("std::shared_ptr<STK_NQ_ZSXX>");
  qRegisterMetaType<std::shared_ptr<ORDER_INFO>>("std::shared_ptr<ORDER_INFO>");
  qRegisterMetaType<std::shared_ptr<MATCH_INFO>>("std::shared_ptr<MATCH_INFO>");
  qRegisterMetaType<std::shared_ptr<ASSET_FUND>>("std::shared_ptr<ASSET_FUND>");
  qRegisterMetaType<std::shared_ptr<ASSET_SHARE>>("std::shared_ptr<ASSET_SHARE>");
  qRegisterMetaType<std::shared_ptr<QUOTE_BACK_INFO>>("std::shared_ptr<QUOTE_BACK_INFO>");
  qRegisterMetaType<std::shared_ptr<LBM_ERROR_INFO>>("std::shared_ptr<LBM_ERROR_INFO>");

  qRegisterMetaType<BOOL>("BOOL");
  qRegisterMetaType<OrderInfoPtr>("OrderInfoPtr");
  qRegisterMetaType<MatchInfoPtr>("MatchInfoPtr");
  qRegisterMetaType<SharedVecPtr<ASSET_FUND>>("SharedVecPtr<ASSET_FUND>");
  qRegisterMetaType<SharedVecPtr<ASSET_SHARE>>("SharedVecPtr<ASSET_SHARE>");
  qRegisterMetaType<SharedVecPtr<QUOTE_ORDER_INFO>>("SharedVecPtr<QUOTE_ORDER_INFO>");
  qRegisterMetaType<SharedVecPtr<QUOTE_MATCH_INFO>>("SharedVecPtr<QUOTE_MATCH_INFO>");
  qRegisterMetaType<std::shared_ptr<ALGOQUOTE_INFO>>("std::shared_ptr<ALGOQUOTE_INFO>");

  QML_REGISTER(Settings);

  // api register
  QML_REGISTER(UserApi);

  /// model register
  QML_REGISTER(AssetCodeModel);
  QML_REGISTER(InfoBarModel);
  QML_REGISTER(MarketBookModel);
  QML_REGISTER(StockListModel);
  QML_REGISTER(OrderInfoModel);
  QML_REGISTER(MatchInfoModel);
  QML_REGISTER(MatchInfoModel);
  QML_REGISTER(AssetFundModel);
  QML_REGISTER(AssetShareModel);
  QML_REGISTER(QuoteOrderInfoModel);
  QML_REGISTER(QuoteMatchInfoModel);
  QML_REGISTER(AlgoStockListModel);

  QML_REGISTER(StockObject);
  QML_REGISTER(AutoQuoteObject);
  qRegisterMetaType<StockObject*>("StockObject*");
  qRegisterMetaType<AutoQuoteObject*>("AutoQuoteObject*");
  qRegisterMetaType<OrderInfo*>("OrderInfo*");
  qRegisterMetaType<MatchInfo*>("MatchInfo*");

  qputenv("QML_DISABLE_DISK_CACHE", "false");

  QQmlApplicationEngine engine;
  engine.load(QUrl("qrc:/Resources/Login.qml"));
  // 登陆成功切换窗口
  QObject::connect(Trader::GetPtr(), &Trader::loginResult, [&](bool result) {
    if(result) {
      QObject* object = engine.rootObjects().first();
      Q_ASSERT(nullptr != object);
      LOG_INFO << object->objectName().toStdString();
      Q_ASSERT("loginWindow" == object->objectName());
      QQuickWindow* loginWindow = qobject_cast<QQuickWindow*>(object);
      Q_ASSERT(loginWindow);
      loginWindow->close();

      LOG_INFO << "login success...";

      ScreenInfo screen_info;
      engine.clearComponentCache();
      engine.addImportPath(QLatin1String("qrc:/"));
      engine.rootContext()->setContextProperty("availableWidth", screen_info.availableWidth);
      engine.rootContext()->setContextProperty("availableHeight", screen_info.availableHeight);
      engine.load(QUrl(QStringLiteral("qrc:/Resources/main.qml")));
      if(!engine.rootContext()) {
        LOG_ERROR << "QQmlApplicationEngine load app.qml failed";
        QMessageBox::warning(nullptr, "ERROR", "QQmlApplicationEngine load app.qml failed");
        qApp->exit(0);
      }
      LOG_INFO << "load main success...";
      QFAppDispatcher* dispatcher = QFAppDispatcher::instance(&engine);
      dispatcher->dispatch("startApp");

      //TTSpeech::GetPtr()->say(QString::fromLocal8Bit("欢迎进入中信做市商自动化交易系统!"));
      LOG_INFO << "----->" << engine.offlineStoragePath().toStdString();
    }
  });

  // TODO 自动登陆
 /* UserApi userApi;
  userApi.userReqLogin("8888", "888888");*/

  return a.exec();
}

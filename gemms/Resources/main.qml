import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0
import QuickFlux 1.1
import "./views"
import "./middlewares"
import "./actions"

Item {

    // 中间件
    MiddlewareList {
        applyTarget: AppActions

        SystemMiddleware {
            mainWindow: mainWindow
        }

        AutoQuoteViewMiddleware { }
    }

    MainWindow {
        id: mainWindow
    }
}


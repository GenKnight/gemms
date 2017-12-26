import QtQuick 2.0
import QuickFlux 1.1
import QtQuick.Dialogs 1.2
import "../actions"
import "../stores"

Middleware {

    property RootStore store: MainStore

    property var mainWindow: null

    function dispatch(type, message) {
        if (type === ActionTypes.startApp) {
            mainWindow.visible = true;
            return;
        }
        next(type, message);
    }

    MessageDialog {
        id: quitConfim
        title: qsTr("确认退出")
        text: qsTr("是否确认退出CEFC自动化做市交易系统\n\n退出将停止所有得策略运行")
        icon: StandardIcon.Question
        modality: Qt.ApplicationModal
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: {
            Qt.quit()
        }
    }

    Connections {
        target: mainWindow
        onClosing: {
            // You may inject a hook to forbid closing or save data if necessary
            close.accepted = false
            quitConfim.open()
        }
    }

}

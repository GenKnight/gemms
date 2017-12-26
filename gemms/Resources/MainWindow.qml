import QtQuick 2.7
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Controls.Material 2.2

import QuickFlux 1.1
import Fluid.Controls 1.0
import Fluid.Layouts 1.0 as FluidLayouts

import "./views"
import "./middlewares"
import "./actions"
import "./stores"
import "./Component"

import qiwookong 1.0

ApplicationWindow {
    id: root
    visible: true
    width: availableWidth
    height: availableHeight
    //    width: 1366
    //    height: 768
    title: qsTr("CEFC-GEMMS 自动化做市交易系统")
    font.family: "微软雅黑"
    Material.primary: Material.LightBlue
    Material.theme: Material.System
    Material.accent: Material.Blue
    decorationColor: Material.primary

    NavigationDrawer {
        id: navDrawer
        //width: Math.min(root.width, root.height) / 3 * 2
        height: root.height
        topContent: [
            Rectangle {
                color: Material.primary
                height: 48
                Label {
                    anchors.centerIn: parent
                    text: qsTr("CEFC 自动化做市")
                }
                Layout.fillWidth: true
            }
        ]
        actions: [
            Action {
                text: qsTr("新增做市面板")
                iconName: "content/add_circle"
                onTriggered: {
                    navDrawer.close()
                    openOrCreateMainQuoteWindow();
                }
            },
            Action {
                text: qsTr("自动重复报价")
                iconName: "content/weekend"
                onTriggered: {
                    navDrawer.close()
                    AppActions.openAutoQuoteView();
                }
            }

        ]
    }

    initialPage: TabbedPage {
        title: root.title
        leftAction: Action {
            iconName: "navigation/menu"
            onTriggered: navDrawer.open()
        }

        actions: [
            Action {
                text: qsTr("做市面板")
                iconName: "content/add_circle"
                toolTip: qsTr("新增做市面板")
                onTriggered: {
                    openOrCreateMainQuoteWindow();
                }
            },
            Action {
                text: qsTr("自动重复报价")
                iconName: "content/weekend"
                toolTip: qsTr("打开自动重复报价策略面板")
                onTriggered: {
                    AppActions.openAutoQuoteView();
                }
            }
        ]

        Tab {
            title: qsTr("个人中心")
            CompositeView {}
        }
    }

    InfoBar {
        id: infoBar
        buttonText: qsTr("")
        buttonColor: "red"
    }

    InfoBarModel {
        id: infoBarModel
        onSigNotice: {
            switch(type) {
            default:
                infoBar.open(qsTr(msg))
                break;
            }
        }
    }

    function openOrCreateMainQuoteWindow() {
        var result = MainStore.mainQuoteStore.openOrCreateMainQuoteView();
         if(!result) {
             infoBar.open(qsTr("可操作股票已经全部打开"))
         }
    }

    Component.onCompleted: {
        root.showMaximized()
    }
}



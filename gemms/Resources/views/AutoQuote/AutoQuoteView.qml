
import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Universal 2.0

import QtQuick.Controls 1.4 as QC14

import Fluid.Controls 1.0
import QuickFlux 1.1

import qiwookong 1.0
import Qt.labs.settings 1.0 as QS

import "../../stores"
import "../../actions"
import "../../Component"
import "../../views"

Window {
    id: root
    width:1366
    height: 768
    title: qsTr("自动重复报价")

    //    maximumHeight: height
    //    minimumHeight: height
    //    maximumWidth: width
    //    minimumWidth: width

    visible: true

    property string currentStock_: ""

    signal appendTotalLog(string log)

    QC14.SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        Rectangle {
            width: 250
            Layout.minimumWidth: 200
            Layout.maximumWidth: 300
            Layout.preferredHeight: parent.height

            AutoQuoteStockList {
                id: autoQuoteStockList
                onEnterAutoQuote: {
                    if(currentStock_ === stock) {
                        return;
                    }
                    settings.row = row;
                    var autoQuoteStockView = MainStore.autoQuoteStockViewStore.getOrCreateAutoQuoteView(row, stock)
                    if(null === autoQuoteStockView.parent) {
                        autoQuoteStockView.parent = stackView;
                        autoQuoteStockView.appendLog.connect(appendTotalLog);
                        autoQuoteStockView.infoBarMsg.connect(infoBar.open);
                    }
                    stackView.replace(autoQuoteStockView, StackView.ReplaceTransition);
                    currentStock_ = stock;
                }
            }
        }

        Rectangle {
            id: autoQuoteView
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height

            QC14.SplitView {
                anchors.fill: parent
                orientation: Qt.Vertical

                Rectangle {
                    height: parent.height / 2
                    Layout.preferredWidth: parent.width
                    Layout.minimumHeight: parent.height / 2
                    Layout.preferredHeight: parent.height / 2
                    Layout.maximumHeight: parent.height / 2

                    StackView {
                        id: stackView
                        anchors.fill: parent
                        initialItem: emptyComponent
                    }
                }

                Rectangle {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true

                    QC14.TabView {
                        anchors.fill: parent
                        style: CTabViewStyle {}

                        QC14.Tab {
                            title : "总日志详情"
                            anchors.fill: parent
                            CTextArea {
                                id: allLogTextArea
                                Component.onCompleted: {
                                    appendTotalLog.connect(allLogTextArea.preAppend);
                                }
                            }
                        }
                    }
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
                    if(code === root.stock) {
                        infoBar.open(code + " " + error_msg)
                    }
                }
            }
        }
    }

    Component {
        id: emptyComponent

        Item {
            Text {
                anchors.centerIn: parent
                text: qsTr("欢迎进入策略下单自动化面板, 没有可以操作得股票")
                font.pixelSize: 24
                font.family: "黑体"
                font.wordSpacing: 10
            }
        }
    }

    QS.Settings {
        id: settings

        property int row: 0
    }

    Component.onCompleted: {
        autoQuoteStockList.setCurrentIndex(settings.row)
    }
}





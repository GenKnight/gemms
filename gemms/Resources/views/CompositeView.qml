import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Window 2.3

import Fluid.Controls 1.0
import Fluid.Layouts 1.0 as FluidLayouts
import QtQuick.Controls 1.4

import QtQuick.Dialogs 1.2

import qiwookong 1.0

import "../Component"
import "./TableView"

Page {
    anchors.fill: parent
    SplitView{
        anchors.fill: parent
        orientation: Qt.Vertical

        // 第一行
        Item {
            height: parent.height / 5
            Layout.minimumHeight: 150
            Layout.preferredWidth: parent.width
            TabView {
                anchors.fill: parent
                style: CTabViewStyle {}

                Tab {
                    title: qsTr("资产")
                    AssetFundTable {  }
                }
            }
        }

        // 第二行
        Item {
            Layout.preferredWidth: parent.width
            Layout.fillHeight: true

            SplitView{
                anchors.fill: parent
                orientation: Qt.Horizontal

                Item {
                    width: parent.width / 2.3
                    Layout.preferredWidth: parent.width / 2.3

                    TabView {
                        anchors.fill: parent
                        style: CTabViewStyle {}
                        Tab {
                            title: qsTr("股票列表")
                            AssetCodeTable { }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true

                    TabView {
                        anchors.fill: parent
                        style: CTabViewStyle {}
                        Tab {
                            title: qsTr("持仓")
                            AssetShareTable { }
                        }
                    }
                }
            }
        }

        // 第三行
        Item {
            height: parent.height / 2
            Layout.minimumHeight: 100
            Layout.maximumHeight: 400
            Layout.preferredWidth: parent.width

            SplitView{
                anchors.fill: parent
                orientation: Qt.Horizontal

                Item {
                    width: parent.width / 2
                    Layout.preferredWidth: parent.width / 2
                    TabView {
                        anchors.fill: parent
                        style: CTabViewStyle {}
                        Tab {
                            title: qsTr("历史委托")

                            HistoryOrderTable {}
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true

                    TabView {
                        anchors.fill: parent
                        style: CTabViewStyle {}
                        Tab {
                            title: qsTr("历史成交")
                            HistoryMatchTable {}
                        }
                    }
                }
            }
        }
    }

    InfoBar {
        id: indexInfoBar
        buttonText: qsTr("")
        buttonColor: "red"
    }

    InfoBarModel {
        onSigNotice: {
            indexInfoBar.open(msg)
        }
    }
}



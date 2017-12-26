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

import "../../stores"
import "../../actions"
import "../../Component"
import "../../views"
import "../TableView"

Item {
    id: assetFundShareTab
    anchors.fill: parent

    property string stock: ""

    QC14.SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        Rectangle {
            width: parent.width
            Layout.minimumHeight: parent.height / 2
            //color: Qt.rgba(Math.random(), Math.random(), Math.random(), 0.1)

            QC14.TabView {
                anchors.fill:parent
                style: CTabViewStyle {}
                QC14.Tab {
                    title: qsTr("持仓")
                    AssetShareTable {
                        stock: assetFundShareTab.stock
                    }
                }

            }
        }

        Rectangle {
            width: parent.width
            Layout.minimumHeight: parent.height /2
            //color: Qt.rgba(Math.random(), Math.random(), Math.random(), 0.1)

            QC14.TabView {
                anchors.fill: parent
                style: CTabViewStyle {}
                QC14.Tab {
                    title: qsTr("资产")
                    AssetFundTable {
                        stock: assetFundShareTab.stock
                    }
                }
            }
        }
    }

}

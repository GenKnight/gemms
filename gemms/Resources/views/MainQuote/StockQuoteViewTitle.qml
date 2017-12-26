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


// 标题栏
Rectangle {
    id: stockQuoteViewTitle
    anchors.fill: parent
    color: "lightBlue"

    property string stock: ""
    property string stockName: ""
    property int titleMargin: 15
    property bool isChanged: false

    signal closeStockQuoteView()

    function updateInvalidTime(time) {
        isChanged = !isChanged;
        invalidTime.text = time;
        if(isChanged) {
            invalidTime.color = "red"
        } else {
            invalidTime.color = "blue"
        }
    }

    function resetInvalidTime() {
        invalidTime.text = "00:00:00";
        invalidTime.color = "black";
    }

    Component.onCompleted: {
        stockQuoteViewTitle.stockName =
                MainStore.stockQuoteListStore.model.getStockName(stockQuoteViewTitle.stock)
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height

            Item {
                anchors.centerIn: parent
                width: titleLabel.contentWidth + invalidTimeLabel.contentWidth + invalidTime.contentWidth + titleMargin
                height: parent.height

                Label {
                    id: titleLabel
                    anchors.verticalCenter: parent.verticalCenter
                    text:  stockQuoteViewTitle.stock + " " + stockQuoteViewTitle.stockName

                    font.family: "黑体"
                    font.pixelSize: 25
                    font.bold: true

                    background: Rectangle {
                         color: "yellow"
                    }
                }

                Label {
                    id: invalidTimeLabel
                    anchors.left: titleLabel.right
                    anchors.leftMargin: titleMargin
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr(" 失效持续 ")
                    font.family: "黑体"
                    font.pixelSize: 15
                    font.bold: true
                }

                Label {
                    id: invalidTime
                    anchors.left: invalidTimeLabel.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("00:00:00")
                    font.family: "黑体"
                    font.pixelSize: 15
                    font.bold: true
                }
            }
        }

        Item {
            id: closeBtn
            Layout.minimumWidth: 80
            Layout.preferredHeight: parent.height

            Button {
                anchors.fill: parent
                anchors.rightMargin: 5
                //highlighted: true
                Material.foreground: Material.Pink
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        closeStockQuoteView()
                    }
                }
                text: qsTr("关闭")
                font.family: "黑体"
            }
        }
    }
}

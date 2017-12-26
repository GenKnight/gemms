import QtQuick 2.0
import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "../../Component"

Rectangle {

    property bool isChanged: false

    signal startBtnClicked()
    signal stopBtnClicked()

    function autoQuoteStarted() {
        startBtn.enabled = false;
        stopBtn.enabled = true;
        statusLabel.text = qsTr("状态: 正在运行")
        statusLabel.background.color = "red"
    }

    function autoQuoteStoped() {
        startBtn.enabled = true;
        stopBtn.enabled = false;
        statusLabel.text = qsTr("状态: 停止运行")
        statusLabel.background.color = "yellow"
    }

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

    Component.onCompleted:  {
        if(null === autoQuoteObject.stockObject) {
            return;
        }
        var stockObject = autoQuoteObject.stockObject;
        titleLabel.text = stockObject.stockName + " " + stockObject.stock + " " + "自动重复报价策略"
    }

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        spacing: 20

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height
            anchors.left: parent.left

            Item {
                id: stockLabel
                anchors.centerIn: parent
                width: titleLabel.contentWidth + invalidTimeLabel.contentWidth + invalidTime.contentWidth + 20
                height: parent.height

                Label {
                    id: titleLabel
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("自动重复报价策略")
                    font.pointSize: 15
                    font.family: "微软雅黑"
                    font.bold: true

                    background: Rectangle {
                         color: "yellow"
                    }
                }

                Label {
                    id: invalidTimeLabel
                    anchors.left: titleLabel.right
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr(" 单边失效持续时间 ")
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
            width: statusLabel.contentWidth
            Layout.preferredHeight: parent.height

            Label {
                id: statusLabel
                anchors.centerIn: parent
                font.family: "楷体"
                font.pixelSize: 15
                text: qsTr("状态: 停止运行")

                background: Rectangle {
                     color: "yellow"
                }
            }
        }

        Item {
            width: 300
            Layout.preferredHeight: parent.height
            anchors.right: parent.right

            RowLayout {
                anchors.fill: parent
                spacing: 20

                Button {
                    id: startBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height
                    width: parent.width / 100
                    text: qsTr("启动")
                    font.family: "微软雅黑"
                    onClicked: {
                        startBtnClicked()
                    }
                }

                Button {
                    id: stopBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height
                    width:10
                    highlighted: true
                    text: qsTr("停止")
                    font.family: "微软雅黑"
                    enabled: false
                    onClicked: {
                        stopBtnClicked()
                    }
                }
            }
        }

    }
}





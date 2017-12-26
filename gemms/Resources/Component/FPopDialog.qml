import QtQuick 2.7
import QtQuick.Controls 2.0
Popup {
    id: root
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    implicitWidth: 300
    implicitHeight: 200

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape

    signal clicked()

    property alias title: titleText.text
    property string text
    property alias okButtonText: okButton.text
    property alias cancleButtonText: cancleButton.text
    property bool cancleButtonVisible: true
    property var okClickFunc: function() {root.close()}
    property var cancleClickFunc: function() {root.close()}
    //将中间部分做成Component属性，外部可以自定义
    property Component contentComponent: Component {
        id: defaultComponent
        Text {
            id: contentText
            anchors.centerIn: parent
            text: root.text
            font.pixelSize: 15
            font.family: "黑体"
        }
    }

    function reset() {
        okButton.text = "确定"
        cancleButton.text = "取消"
        cancleButtonVisible = false
        okClickFunc = function() {root.close()}
        cancleClickFunc = function() {root.close()}
        contentComponent = defaultComponent
    }
    contentItem: Item {
        id: contentItem
        anchors.fill: parent
        anchors.margins: 2
        Rectangle {
            id: titleRect
            anchors.top: parent.top
            anchors.topMargin: 2
            height: 20
            width: parent.width
            radius: 2
            color: "lightBlue"
            Text {
                id: titleText
                anchors.centerIn: parent
                font.bold: true
                font.pixelSize: 20
            }
        }
        Rectangle {
            height: 1
            width: parent.width
            anchors.top: titleRect.bottom
            anchors.topMargin: 2
            color: "black"
        }
        Loader {
            id: centerContent
            anchors.centerIn: parent
            sourceComponent: contentComponent
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
            spacing: 50
            FColorButton {
                id: okButton
                text: "确定"
                onClicked: {
                    root.clicked()
                    root.okClickFunc()
                }
            }
            FColorButton {
                id: cancleButton
                text: "取消"
                visible: root.cancleButtonVisible
                onClicked: {
                    root.cancleClickFunc()
                }
            }
        }
    }
    background: Rectangle {
//        color: "#09556c"
        radius: 5
    }
}

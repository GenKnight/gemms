import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

RowLayout {
    anchors.fill: parent
    spacing: 10

    property alias text: label.text
    property alias from : spinBox.from
    property alias to : spinBox.to
    property alias stepSize: spinBox.stepSize
    property alias value: spinBox.value

    property int decimals: 0

    Item {
        Layout.preferredWidth: parent.width / 5 * 1.8
        Layout.preferredHeight: parent.height

        Text {
            id: label
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: ""
            color: "black"
            font.family: "微软雅黑"
            font.pixelSize: 16
        }
    }

    Item {
        Layout.fillWidth: parent.width
        Layout.preferredHeight: parent.height

        SpinBox {
            id: spinBox
            anchors.fill: parent
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            editable: true

        }

    }
}

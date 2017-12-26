import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4 as QC14
import QtQuick.Controls.Styles 1.4 as QCS14

RowLayout {
    anchors.fill: parent

    property alias text: label.text
    property alias value: value.text
    property alias valueColor: value.color
    property alias textColor: label.color

    Item {
        id: labelRect
        Layout.preferredWidth: parent.width / 2
        Layout.preferredHeight: parent.height

        Text {
            id: label
            anchors.right: parent.right
            text: ""
            font.bold: true
            font.pixelSize: 15
            color: "steelblue"
            font.family: "华文细黑"

        }
    }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: parent.height

        Text {
            id: value
            anchors.left: parent.left
            text: ""
            font.bold: true
            font.pixelSize: 15
            color: "grey"
            font.family: "华文细黑"
        }
    }
}

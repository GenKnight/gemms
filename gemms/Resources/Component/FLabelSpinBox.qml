import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4 as QC14
import QtQuick.Controls.Styles 1.4 as QCS14

RowLayout {
    anchors.fill: parent
    spacing: 5

    property alias text: label.text
    property alias decimals: spinBox.decimals
    property alias minimumValue: spinBox.minimumValue
    property alias maximumValue: spinBox.maximumValue
    property alias stepSize: spinBox.stepSize
    property alias value: spinBox.value
    property alias horizontalAlignment: spinBox.horizontalAlignment

    signal sigEditingFinished(int value)

    Text {
        id: label
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        text: ""
        color: "steelblue"
        font.bold: true
        font.family: "黑体"
        font.pixelSize: 15
    }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: parent.height

        QC14.SpinBox {
            id: spinBox
            anchors.fill: parent
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            selectByMouse: true
            value: 0.00
            maximumValue: 1000000000
            font.bold: true
            font.family: "黑体"
            font.pixelSize: 15
            horizontalAlignment: Qt.AlignHCenter

            onEditingFinished: {
                sigEditingFinished(spinBox.value);
            }
        }
    }
}

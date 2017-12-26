import QtQuick 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "qrc:/resources/src/buttons"

RowLayout {

    anchors.fill: parent

    Row {
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5

        ButtonDefault {
            class_name: "calm"
            width: 100
            height: 30
            text: "查询"
        }
    }
}

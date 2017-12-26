import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

MenuItem {

    id: root
    property alias text: root.text

    signal cancelTriggered()

    text: qsTr("撤单")
    shortcut: StandardKey.Delete

    onTriggered: {
        cancelTriggered()
    }
}

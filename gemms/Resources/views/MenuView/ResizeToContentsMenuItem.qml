import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

MenuItem {
    id: root
    property alias text: root.text

    signal resizeToContentsTriggered()

    text: qsTr("自动调整列宽")
    shortcut: StandardKey.UnknownKey

    onTriggered: {
        resizeToContentsTriggered()
    }
}

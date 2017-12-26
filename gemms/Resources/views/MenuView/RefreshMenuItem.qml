import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

MenuItem {
    id: root
    property alias text: root.text

    signal refreshTriggered()

    text: qsTr("刷新")
    shortcut: StandardKey.Refresh

    onTriggered: {
        refreshTriggered()
    }
}

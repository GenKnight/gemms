import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: root
    width: 200
    height: 200

    signal deleteTriggered

    Menu {
        id: commonMenu


        MenuItem {
            id: deleteAction
            text: qsTr("删除")
            shortcut: StandardKey.Delete

            onTriggered: {
                root.deleteTriggered()
            }
        }
    }

    function popup() {
        commonMenu.popup()
    }


}



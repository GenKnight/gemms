import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import qiwookong 1.0

import "../../Component"
import "../MenuView"

FTable {
    id: root

    property string stock: ""

    model: AssetShareModel {
        id: assetShareModel
        Component.onCompleted: {
            root.resizeAllColumns()
        }
    }

    ContextMenu {
        id: contextMenu


        RefreshMenuItem {
            onRefreshTriggered: {
                assetShareModel.refresh(root.stock)
            }

        }
    }

    onRightBtnClicked: {
        contextMenu.popup()
    }

    Component.onCompleted: {
        assetShareModel.refresh(root.stock)
    }
}

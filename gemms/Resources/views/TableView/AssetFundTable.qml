import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import qiwookong 1.0

import "../../Component"
import "../MenuView"

// 委托回报表
FTable {
    id: root

    property string stock: ""

    model: AssetFundModel {
        id: assetFundModel
        Component.onCompleted: {
            root.resizeAllColumns()
        }
    }

    ContextMenu {
        id: contextMenu

        RefreshMenuItem {
            onRefreshTriggered: {
                assetFundModel.refresh(root.stock)
            }
        }
    }

    onRightBtnClicked: {
        contextMenu.popup()
    }

    Component.onCompleted: {
        assetFundModel.refresh(root.stock)
    }
}

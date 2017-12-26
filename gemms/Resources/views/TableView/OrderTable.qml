import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import qiwookong 1.0

import "../../Component"
import "../MenuView"

// 委托回报表
FTable {
    id: root
    anchors.fill: parent

    property alias stock: orderInfoModel.stock

    model: OrderInfoModel {
        id: orderInfoModel
        Component.onCompleted: {
            root.resizeAllColumns()
        }
    }

    ContextMenu {
        id: contextMenu

        CancelOrderMenuItem {
            onCancelTriggered: {
                orderInfoModel.cancelOrder(root.currentRow)
            }
        }
    }

    onRightBtnClicked: {
        contextMenu.popup()
    }
}

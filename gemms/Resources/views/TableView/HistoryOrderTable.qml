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

    model: QuoteOrderInfoModel {
        id: quoteOrderInfoModel
        Component.onCompleted: {
            root.resizeAllColumns()
        }
    }

    ContextMenu {
        id: contextMenu

        CancelOrderMenuItem {
            onCancelTriggered: {
                quoteOrderInfoModel.cancelOrder(root.currentRow)
            }
            visible: root.currentRow >= 0
        }

        MenuSeparator {
            visible: root.currentRow >= 0
        }

        RefreshMenuItem {
            onRefreshTriggered: {
                quoteOrderInfoModel.refresh(root.stock)
            }


        }
    }

    onRightBtnClicked: {
        contextMenu.popup()
    }

    Component.onCompleted: {
        quoteOrderInfoModel.refresh(root.stock)
    }
}

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

    model: QuoteMatchInfoModel {
        id: quoteMatchInfoModel
        Component.onCompleted: {
            root.resizeAllColumns()
        }
    }
    ContextMenu {
        id: contextMenu

        RefreshMenuItem {
            id: refreshMenuItem

            onRefreshTriggered: {
                quoteMatchInfoModel.refresh(stock)
            }
        }
    }

    onRightBtnClicked: {
        contextMenu.popup()
    }

    Component.onCompleted: {
        quoteMatchInfoModel.refresh(root.stock)
    }
}

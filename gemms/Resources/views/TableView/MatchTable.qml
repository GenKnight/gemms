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

    property alias stock: matchInfoModel.stock

    model: MatchInfoModel {
        id: matchInfoModel
        Component.onCompleted: {
            root.resizeAllColumns()
        }
    }

}

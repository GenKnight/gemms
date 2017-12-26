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

    model: AssetCodeModel {
        id: assetCodeModel
        Component.onCompleted: {
            root.resizeAllColumns()
        }
    }
}

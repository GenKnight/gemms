import QtQuick 2.0
import QuickFlux 1.1

import "../actions"
import "../Component"

import qiwookong 1.0

Store {

    property alias model: algoStockListModel
    AlgoStockListModel {
        id: algoStockListModel

    }
}

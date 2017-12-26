import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Universal 2.2

import Fluid.Controls 1.0

import "../actions"
import "../stores"
import "../Component"
import qiwookong 1.0

Rectangle {
    anchors.fill: parent
    color: "black"

    // 文本
    property alias title: title.text
    property alias model: listView.model
    property StockObject marketBookStockObject: null
    property bool canClicked: true

    // 每个book的高度
    property real bookHeight: (parent.height - head.height) / 21
    property var askClickItem: null
    property var bidClickItem: null

    property double dAskPrice_: 0.0
    property double dBidPrice_: 0.0

    // 点击book事件
    signal marketDataClick(int index, double price, int vol)

    function resetMarketBookColor() {
        listView.model.resetModel();
    }

    Component.onCompleted: {
        // 做市book变化
        if(MarketBookModel.ZSMarketType === listView.model.type) {
            marketBookStockObject.orderInfoChanged.connect(resetMarketBookColor);
        }
    }

    Rectangle {
        id: head
        width: parent.width
        height: 20
        anchors.top: parent.top

        Text {
            id: title
            anchors.centerIn: parent
            font.bold: true
            font.pixelSize: 13
        }
    }


    ListView {
        id: listView
        anchors {
            top: head.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        interactive: false

        delegate: ListItem {
            width: parent.width
            height: bookHeight

            Rectangle {
                id: marketItem
                anchors.fill: parent
                color: {
                    // 如果非做市行情，直接黑色
                    marketItem.isQuoteItem = false;
                    if(MarketBookModel.ZSMarketType !== listView.model.type) {
                        return "balck"
                    }

                    // 没有委托黑色
                    var orderInfo = marketBookStockObject.orderInfo;
                    if(null === orderInfo) {
                        return "black";
                    }

                    // 如果当前行情和委托行情价个一样，则背景变色
                    if(Number(model.Price) === orderInfo.dAskPrice
                            || Number(model.Price) === orderInfo.dBidPrice) {

                        // 如果做市行情有委托
                        if( orderInfo.bAllMatched) {
                            // 如果委托全部成交,变蓝色，需要重新报单
                            return "blue";
                        } else {
                            marketItem.isQuoteItem = true;
                            return "red";
                        }
                    } else {
                        return "black";
                    }
                }

                property int index: -1
                property bool isQuoteItem: false

                Row {
                    anchors.fill: parent
                    visible: 10 !== index

                    Item {
                        width: parent.width / 3
                        height: parent.height
                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: generateBookLabel(index, model.price, model.vol)
                            color: "white"
                            font.pixelSize: 15
                            font.family: "黑体"
                            //font.bold: true
                        }
                    }

                    Item {
                        width: parent.width / 3
                        height: parent.height
                        Text {
                            id: currPrice
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: model.Price
                            color: "white"
                            font.pixelSize: 15
                            font.family: "黑体"
                            visible: {
                                if(0.0 == Number(model.Price)) {
                                    return false;
                                } else {
                                    return true;
                                }
                            }
                        }
                    }

                    Item {
                        width: parent.width / 3
                        height: parent.height
                        Text {
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            text: model.Volume
                            color: "yellow"
                            font.pixelSize: 15
                            font.family: "黑体"       
                            visible: {
                                if(0 == Number(model.Price)) {
                                    return false;
                                } else {
                                    return true;
                                }
                            }
                        }
                    }
                }

                /// 分隔符
                Rectangle {
                    width: parent.width
                    height: 2
                    anchors.centerIn: parent
                    color: "yellow"
                    visible: 10 === index
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        marketItem.index= index
                        marketDataClick(index, model.Price, model.Volume)

                        if(!marketItem.isQuoteItem) {
                            clickAnimation.start();
                        }

                    }
                }

                SequentialAnimation {
                    id: clickAnimation

                    ColorAnimation {
                        target: marketItem
                        property: "color"
                        from: "black"
                        to: "orange"
                        duration: 300
                    }

                    NumberAnimation {
                        duration: 200
                    }

                    ColorAnimation {
                        target: marketItem
                        property: "color"
                        from: "orange"
                        to: "black"
                        duration: 500
                    }
                }
            }
        }
    }



    function generateBookLabel(index,price,vol) {
        switch(index) {
        case 0:return qsTr("卖⑩")
        case 1:return qsTr("卖⑨")
        case 2:return qsTr("卖⑧")
        case 3:return qsTr("卖⑦")
        case 4:return qsTr("卖⑥")
        case 5:return qsTr("卖⑤")
        case 6:return qsTr("卖④")
        case 7:return qsTr("卖③")
        case 8:return qsTr("卖②")
        case 9:return qsTr("卖①")
        case 11:return qsTr("买①")
        case 12:return qsTr("买②")
        case 13:return qsTr("买③")
        case 14:return qsTr("买④")
        case 15:return qsTr("买⑤")
        case 16:return qsTr("买⑥")
        case 17:return qsTr("买⑦")
        case 18:return qsTr("买⑧")
        case 19:return qsTr("买⑨")
        case 20:return qsTr("买⑩")
        default: return ""
        }
    }
}



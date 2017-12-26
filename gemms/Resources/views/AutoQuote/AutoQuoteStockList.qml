import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Fluid.Controls 1.0
import qiwookong 1.0
import QtQuick.Controls 1.4

import "../../stores"
import "../../actions"

Rectangle {
    anchors.fill: parent
    border.color: "lightblue"

    property AutoQuoteObject currAutoQuoteObject: null

    signal enterAutoQuote(int row, string stock)

    function setCurrentIndex(row) {
        if(row < listView.count) {
            listView.currentIndex = row;
            currAutoQuoteObject = MainStore.autoQuoteStockListStore.model.getAutoQuoteObject(row)
            if(currAutoQuoteObject) {
                enterAutoQuote(row, currAutoQuoteObject.stock)
            }
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: MainStore.autoQuoteStockListStore.model

        // 视图是否负责高亮代理的移动
        highlightFollowsCurrentItem: false
        highlightMoveDuration: 200

        currentIndex: -1
        ScrollBar.vertical: ScrollBar {}

        delegate: Component {
            Loader {
                width: parent.width

                sourceComponent: Component {
                    ListItem {
                        width: parent.width
                        height: 40
                        highlighted: ListView.isCurrentItem

                        Item {
                            anchors.fill: parent

                            Text {
                                anchors.centerIn: parent
                                text: display
                                color: {
                                    if(model.isRunning) {
                                        return "red"
                                    } else {
                                        return "black"
                                    }
                                }
                                font.pointSize: 11
                                font.family: "黑体"
                            }
                        }

                        onClicked: {
                            listView.currentIndex = index
                            enterAutoQuote(index, model.szStkCode)
                        }
                    }
                }
            }
        }

        highlight: Rectangle {
            id: highlightRectangle
            width: ListView.view.width
            height: ListView.view.currentItem.height
            y: ListView.view.currentItem.y

            color: "lightblue"
            //color: "#9c27b0"

            Behavior on y {
                SequentialAnimation {
                   PropertyAnimation {
                       target: highlightRectangle;
                       property: "opacity"
                       to: 0
                       duration: 200
                   }

                   NumberAnimation {
                       duration: 1
                   }

                   PropertyAnimation {
                       target: highlightRectangle
                       property: "opacity"
                       to: 1
                       duration: 200
                   }
                }
            }
        }
    }

}

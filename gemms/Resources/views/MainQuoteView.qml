import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.LocalStorage 2.0

import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Universal 2.2

import Fluid.Controls 1.0
import Fluid.Layouts 1.0

import qiwookong 1.0

import "../actions"
import "../stores"
import "../Component"
import "qrc:/Resources/script/Database.js" as DB

import qiwookong 1.0

Window {
    id: root
    title: qsTr("CEFC 做市面板")
    width: availableWidth
    height: availableHeight
//    width: 1366
//    height: 768

    property bool isNew: false
    property var dataArray: []
    property int appId: 0
    property string state: "1"

    Item {
        anchors.fill: parent

        signal sigClosing()

        Flickable {
            clip: true
            anchors.fill: parent
            contentHeight: Math.max(layout.implicitHeight, height)

            ScrollBar.vertical: ScrollBar {}

            ColumnFlow {
                id: layout
                anchors.fill: parent

                model: 8
                columnWidth: parent.width / 4
                contentHeight: parent.height / 2

                delegate: Rectangle {
                    id: stockQuoteItem
                    height: parent.height / 2
                    border.width: 2
                    border.color: "lightBlue"

                    Loader {
                        id: emptyLoader
                        Connections {
                            target: emptyLoader.item
                            onAddClicked: {
                                stockListLoader.source = "qrc:/Resources/views/MainQuote/StockQuoteList.qml";
                                stackView.replace(stockListLoader)
                            }
                        }
                    }

                    Loader {
                        id: stockListLoader
                        Connections {
                            target: stockListLoader.item
                            onChooseStock: {
                                DB.cacheHandle(index, stock)
                                openOrCreateStockQuoteView(stock)
                            }
                        }
                    }

                    StackView {
                        id: stackView
                        anchors.fill: parent
                        initialItem: emptyLoader
                        Component.onCompleted: {
                            var data = root.dataArray[index]
                            if(0 !== data.stock.length) {
                                openOrCreateStockQuoteView(data.stock)
                                return;
                            }
                            emptyLoader.sourceComponent = emptyComponent
                        }
                    }

                    function openOrCreateStockQuoteView(stock) {
                        // 创建或者获取缓存得面板
                        var stockQuoteView =  MainStore.stockQuoteViewStore.openOrCreateStockQuoteView(stock)
                        if(null !== stockQuoteView) {
                            // 首次创建，设置 parent,关联信号
                            connections.target = stockQuoteView
                            if(stockQuoteItem !== stockQuoteView.parent) {
                                stockQuoteView.parent = stockQuoteItem
                            }
                            stackView.replace(stockQuoteView);
                            MainStore.stockQuoteListStore.model.remove(stock);
                        }
                    }

                    Connections {
                        id: connections
                        target: null
                        onStockQuoteClosing: {
                            connections.target = null
                            MainStore.stockQuoteListStore.model.add(stock);
                            DB.removeHandle(index)
                            emptyLoader.sourceComponent = emptyComponent;
                            stackView.replace(emptyLoader);
                        }
                    }
                }
            }
        }

        Component {
            id: emptyComponent
            Item {
                signal addClicked()
                Button {
                    anchors.centerIn: parent
                    highlighted: true
                    Material.accent: Material.LightBlue
                    text: qsTr("+")
                    font.pixelSize: 25
                    onClicked: {
                        if(MainStore.stockQuoteListStore.model.rowCount() <= 0) {
                            emptyInfoBar.open(qsTr("可操作股票已经全部打开"));
                        } else {
                            addClicked()
                        }
                    }
                }

                InfoBar {
                    id: emptyInfoBar
                }
            }
        }
    }

    // 界面等待资源加载完毕
    BusyIndicator {
        anchors.centerIn: parent
        running: Loader.Null === initLoader.status
                 || Loader.Loading === initLoader.status
    }

    Loader {
        id: initLoader
        sourceComponent: Component {
            Item {
                Component.onCompleted: {
                    root.appId = MainStore.mainQuoteStore.getCurrQuoteId()
                    DB.initDatabase()
                    DB.readData()
                }
            }
        }
    }

    Component.onDestruction: {
        DB.storeData()
    }
}

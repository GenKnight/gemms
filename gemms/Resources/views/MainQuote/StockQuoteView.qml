import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Universal 2.0

import QtQuick.Controls 1.4 as QC14

import Fluid.Controls 1.0
import QuickFlux 1.1

import qiwookong 1.0

import "../../stores"
import "../../actions"
import "../../Component"
import "../../views"

Rectangle {
    id: stockQuoteView

    anchors.margins: 2
    radius: 5
    border.width: 2

    // 股票代码
    property string stock: ""
    property StockObject stockObject: null

    /// 关闭按钮
    signal stockQuoteClosing(string stock)

    function slotOrderInfoChanged() {
        var orderInfo = stockObject.orderInfo;
        if(null === orderInfo) {
            return;
        }

        // 如果单边挂单小于1000， 或者全部成交，视为失效
        if(orderInfo.bAllMatched
                || orderInfo.dAskVol < 1000
                || orderInfo.dBidVol < 1000) {

            timer.startTimer();
        } else {
            timer.stopTimer();
        }
    }

    Component.onCompleted: {
        // 获取所有该股票相关信息
        stockObject = MainStore.stockQuoteListStore.model.getStockObjectByStock(stock)

        stockObject.sigStockQuoteError.connect(infoBar.open)
        stockObject.orderInfoChanged.connect(slotOrderInfoChanged);
    }

    CountTimer {
        id: timer
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 2
        anchors.margins: 2

        // 标题栏
        Item {
            Layout.preferredWidth: parent.width
            Layout.minimumHeight: 35
            StockQuoteViewTitle {
                stock: stockQuoteView.stock
                onCloseStockQuoteView: {
                    stockQuoteClosing(stockQuoteView.stock);
                }

                Component.onCompleted: {
                    timer.timerTriggered.connect(updateInvalidTime);
                    timer.timerStop.connect(resetInvalidTime);
                }
            }
        }

        // 窗口
        Item {
            Layout.preferredWidth: parent.width
            Layout.fillHeight: true

            QC14.TabView {
                anchors.fill: parent
                style: CTabViewStyle {}

                QC14.Tab {
                    title: qsTr("行情信息")
                    anchors.fill: parent
                    Item {
                        anchors.fill: parent
                        anchors.topMargin: 5
                        StockQuoteTab {
                            stock: stockQuoteView.stock
                        }
                    }
                }

                QC14.Tab {
                    title: qsTr("委托成交")
                    anchors.fill: parent
                    Item {
                        anchors.fill: parent
                        anchors.topMargin: 5
                        OrderMatchTab {
                            stock: stockQuoteView.stock
                        }
                    }
                }

                QC14.Tab {
                    id: historyTab
                    title: qsTr("挂单详情")
                    anchors.fill: parent
                    Item {
                        anchors.fill: parent
                        anchors.topMargin: 5
                        HistoryOrderMatchTab {
                            stock: stockQuoteView.stock
                        }
                    }
                }

                QC14.Tab {
                    title: qsTr("资产持仓")
                    anchors.fill: parent
                    Item {
                        anchors.fill: parent
                        anchors.topMargin: 5
                        AssetFundShareTab {
                            stock: stockQuoteView.stock
                        }
                    }
                }
            }
        }
    }

    InfoBar {
        id: infoBar
        buttonText: qsTr("")
        buttonColor: "red"
    }

    InfoBarModel {
        id: infoBarModel
        onSigNotice: {
            if(code === stockQuoteView.stock) {
                switch(type) {
                default:
                    infoBar.open(qsTr(msg))
                    break;
                }
            }
        }
    }
}


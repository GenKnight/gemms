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

Item {
    id: stockQuoteTab
    anchors.fill: parent

    property string stock: ""

    RowLayout {
        anchors.fill: parent
        spacing: 5

        Item {
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 3
            Layout.preferredHeight: parent.height
            Layout.maximumWidth: parent.width / 3

            MarketBookList {
                title: qsTr("做市10档行情")
                marketBookStockObject: stockObject
                model: MarketBookModel {
                    type: MarketBookModel.ZSMarketType
                    stock: stockQuoteTab.stock
                }

                Component.onCompleted: {
                    marketDataClick.connect(operationPane.onMarketDataClicked)
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 3
            Layout.preferredHeight: parent.height
            Layout.maximumWidth: parent.width / 3

            MarketBookList {
                title: qsTr("投资10档行情")
                marketBookStockObject: stockObject
                canClicked: false
                model: MarketBookModel {
                    type: MarketBookModel.TZMarketType
                    stock: stockQuoteTab.stock
                }

                Component.onCompleted: {
                    marketDataClick.connect(operationPane.onMarketDataClicked)
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 3
            Layout.maximumWidth: parent.width / 3
            Layout.preferredHeight: parent.height

            Item {
                anchors.fill: parent
                anchors.rightMargin: 3

                StockQuoteOperationPane {
                    id: operationPane
                    anchors.fill: parent

                    Component.onCompleted: {
                        quoteConfimDialog.clicked.connect(quoteConfimOk)
                    }
                }
            }
        }
    }

    FPopDialog {
        id: quoteConfimDialog
    }

    function openConfimDialog(title, text) {
        quoteConfimDialog.title = title;
        quoteConfimDialog.text = text;
        quoteConfimDialog.open()
    }
}

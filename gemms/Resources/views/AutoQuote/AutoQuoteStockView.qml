
import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQml 2.2

import QtQuick.Controls 1.4 as QC14
import QtQuick.Controls 2.2 as QC22

import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Universal 2.0


import Fluid.Controls 1.0
import QuickFlux 1.1

import qiwookong 1.0

import "../../stores"
import "../../actions"
import "../../Component"
import "../../views"

Item {
    id: autoQuoteStockView

    property int row: 0
    property string stock: ""
    property AutoQuoteObject autoQuoteObject: null

    readonly property string paramsKey: autoQuoteStockView.stock + "," + "autoQuoteInterval"

    signal appendLog(string log)
    signal infoBarMsg(string msg)

    // 启动自动化做市
    function startAutoQuote() {
        autoQuoteObject.startAutoQuote();
    }

    // 停止自动化做市
    function stopAutoQuote() {
        autoQuoteObject.stopAutoQuote();
    }

    // 运行状态
    function setRunningState() {
        autoQuoteSettingsGroupbox.checked = false;
        autoQuoteViewTitle.autoQuoteStarted();
    }

    // 停止状态
    function setStopState() {
        autoQuoteViewTitle.autoQuoteStoped();
        autoQuoteSettingsGroupbox.checked = true;
    }

    function reset() {
        showAskStatus.value =   "--"
        showAskPrice.value =    "--"
        showAskWtVol.value =    "--"
        showAskVol.value =      "--"
        showBidStatus.value =   "--"
        showBidPrice.value  =   "--"
        showBidWtVol.value  =   "--"
        showBidVol.value =      "--"
    }

    function orderInfoChanged(orderInfo) {
        // 双标报价成功则赋值
        showAskStatus.value = orderInfo.askOrderStatus
        showAskPrice.value = orderInfo.dAskPrice
        showAskWtVol.value = orderInfo.nAskWtVol
        showAskVol.value = orderInfo.nAskVol
        showBidStatus.value = orderInfo.bidOrderStatus
        showBidPrice.value = orderInfo.dBidPrice
        showBidWtVol.value = orderInfo.nBidWtVol
        showBidVol.value = orderInfo.nBidVol


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
        autoQuoteObject = MainStore.autoQuoteStockListStore.model.getAutoQuoteObject(autoQuoteStockView.row)
        if(null === autoQuoteObject) {
            return;
        }

        // 运行状态变化
        autoQuoteObject.runningChanged.connect(function(row) {
            if(!autoQuoteObject.isRunning) {
                setStopState();
                stopAutoQuote();
            } else {
                setRunningState();
            }
        })

        // 错误状态
        autoQuoteObject.sigAutoQuoteError.connect(function(error){
            infoBarMsg(error);
        });

        // 日志
        autoQuoteObject.sigAppendLog.connect(function(msg) {
            logArea.preAppend(msg)
            appendLog(msg)
        });

        // 委托状态
        autoQuoteObject.sigOrderInfoChanged.connect(function(orderInfo){
            orderInfoChanged(orderInfo);
        });

        quoteIntervalBox.value = settings.value(paramsKey, 120)
    }

    CountTimer {
        id: timer
    }

    Settings {
        id: settings
    }

    MessageDialog {
        id: stopConfim
        title: qsTr("确认停止")
        text: qsTr("是否确认暂停策略执行")
        icon: StandardIcon.Question
        modality: Qt.WindowModal
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: {
            stopAutoQuote()
        }
        onRejected: {
            autoQuoteSettingsGroupbox.checked = false
        }
    }

    AutoQuoteViewTitle {
        id: autoQuoteViewTitle
        width: parent.width
        height: 35

        onStartBtnClicked: {
            startAutoQuote()
        }

        onStopBtnClicked: {
            stopAutoQuote()
        }

        Component.onCompleted: {
            timer.timerTriggered.connect(updateInvalidTime);
            timer.timerStop.connect(resetInvalidTime);
        }
    }


    RowLayout {
        anchors {
            top: autoQuoteViewTitle.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        Rectangle {
            width: 500
            Layout.maximumWidth: parent.width / 2
            Layout.preferredHeight: parent.height

            QC14.GroupBox {
                id: autoQuoteSettingsGroupbox
                anchors.fill: parent

                checkable: true
                checked: true
                title: qsTr("策略配置")

                onCheckedChanged: {
                    if(!autoQuoteObject.isRunning && !checked) {
                        checked = true
                    }

                    if(autoQuoteObject.isRunning && checked) {
                        stopConfim.open()
                    }
                }

                RowLayout {
                    anchors.fill: parent

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredWidth: parent.width / 3
                        Layout.preferredHeight: parent.height
                        Layout.maximumWidth: parent.width / 3

                        MarketBookList {
                            title: qsTr("做市10档行情")
                            canClicked: false
                            marketBookStockObject: autoQuoteObject.stockObject
                            model: MarketBookModel {
                                type: MarketBookModel.ZSMarketType
                                stock: autoQuoteStockView.stock
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
                            canClicked: false
                            marketBookStockObject: autoQuoteObject.stockObject
                            model: MarketBookModel {
                                type: MarketBookModel.TZMarketType
                                stock: autoQuoteStockView.stock
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredWidth: parent.width / 3
                        Layout.maximumWidth: parent.width / 3
                        Layout.preferredHeight: parent.height

                        QC14.GroupBox {
                            title: qsTr("当前买卖状态")
                            anchors.fill: parent

                            ColumnLayout {
                                anchors.topMargin: 15
                                anchors.bottomMargin: 10
                                anchors.fill: parent
                                spacing: 5

                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true

                                    FLabelSpinBox {
                                        id: quoteIntervalBox
                                        text: qsTr("间隔(s):")
                                        value: 120
                                        decimals: 0
                                        stepSize: 1
                                        minimumValue: 5
                                        maximumValue: 300
                                        horizontalAlignment: Qt.AlignHCenter

                                        onSigEditingFinished: {
                                            settings.setValue(paramsKey, value)
                                        }
                                    }
                                }

                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true

                                    FLabelValue {
                                        id: showAskStatus
                                        text: qsTr("叫卖状态: ")
                                        value: "- -"
                                    }
                                }

                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true

                                    FLabelValue {
                                        id: showAskVol
                                        text: qsTr("叫卖挂单: ")
                                        value: "--"
                                    }
                                }

                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true
                                    FLabelValue {
                                        id: showAskPrice
                                        text: qsTr("叫卖价: ")
                                        value: "- -"
                                    }
                                }

                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true
                                    FLabelValue {
                                        id: showAskWtVol
                                        text: qsTr("叫卖数量: ")
                                        value: "- -"
                                    }
                                }


                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true

                                    FLabelValue {
                                        id: showBidStatus
                                        text: qsTr("叫买状态: ")
                                        value: "- -"
                                    }
                                }

                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true
                                    FLabelValue {
                                        id:  showBidVol
                                        text: qsTr("叫买挂单: ")
                                        value: "--"
                                    }
                                }

                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true
                                    FLabelValue {
                                        id: showBidPrice
                                        text: qsTr("叫买价: ")
                                        value: "- -"
                                    }
                                }

                                Item {
                                    Layout.preferredWidth: parent.width
                                    Layout.fillHeight: true
                                    FLabelValue {
                                        id: showBidWtVol
                                        text: qsTr("叫买数量: ")
                                        value:"- -"
                                        valueColor: "grey"
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height

            QC14.GroupBox {
                anchors.fill: parent

                title: qsTr("策略监控")

                CTextArea {
                    id: logArea
                }
            }
        }

    }

    ListModel {
        id: buyListModel
        ListElement { key: "请选择" }
        ListElement { key: "买①挡" }
        ListElement { key: "买②挡" }
        ListElement { key: "买③挡" }
        ListElement { key: "买④挡" }
        ListElement { key: "买⑤挡" }
        ListElement { key: "买⑥挡" }
        ListElement { key: "买⑦挡" }
        ListElement { key: "买⑧挡" }
        ListElement { key: "买⑨挡" }
        ListElement { key: "买⑩挡" }
    }

    ListModel {
        id: sellListModel
        ListElement { key: "请选择" }
        ListElement { key: "卖⑩挡" }
        ListElement { key: "卖⑨挡" }
        ListElement { key: "卖⑧挡" }
        ListElement { key: "卖⑦挡" }
        ListElement { key: "卖⑥挡" }
        ListElement { key: "卖⑤挡" }
        ListElement { key: "卖④挡" }
        ListElement { key: "卖③挡" }
        ListElement { key: "卖②挡" }
        ListElement { key: "卖①挡" }
    }

}

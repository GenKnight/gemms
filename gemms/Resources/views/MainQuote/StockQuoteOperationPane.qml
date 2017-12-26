import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Universal 2.0

import QtQuick.Controls 1.4 as QC14
import QtQuick.Controls.Styles 1.4 as QCS14

import QuickFlux 1.0
import Fluid.Controls 1.0

import qiwookong 1.0

import "../../stores"
import "../../actions"
import "../../Component"

ColumnLayout {

    anchors.fill: parent
    spacing: 5

    property alias dAskPrice: askPrice.value
    property alias nAskVol: askVol.value
    property alias stepValue: stepBox.value
    property alias dBidPrice: bidPrice.value
    property alias nBidVol: bidVol.value

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

    function quote() {
        if(!dAskPrice) {
            infoBar.open(qsTr("请设置合理的卖出价格!"))
            return
        }
        if(!nAskVol) {
            infoBar.open(qsTr("请先设置合理的卖出量!"))
            return
        }
        if(!dBidPrice) {
            infoBar.open(qsTr("请先设置合理的买入价!"))
            return
        }
        if(!nBidVol) {
            infoBar.open(qsTr("请先设置合理的买入量!"))
            return
        }

        var notice = qsTr("交易操作: 做市双边报价\n")
        notice += qsTr("股票代码: ") + stockObject.stock + "\n"
        notice += qsTr("股票名称: ") + qsTr(stockObject.stockName) + "\n"
        notice += qsTr("买入价格: ") + dBidPrice + "\n"
        notice += qsTr("买入数量: ") + nBidVol + "\n"
        notice += qsTr("卖出价格: ") + dAskPrice + "\n"
        notice += qsTr("卖出数量: ") + nAskVol + "\n"
        openConfimDialog(qsTr("确认"), notice);
    }

    // 确认下单
    function quoteConfimOk() {
        console.debug("QuoteByHand 2 " + dAskPrice + ":" + nAskVol + ", " + dBidPrice + ":" + nBidVol);
        stockObject.algoQuote(dAskPrice, nAskVol, dBidPrice, nBidVol);
    }

    function cancelOrder() {
        stockObject.cancelOrder();
    }

    function onMarketDataClicked(index, price, vol) {
        if(index < 10) {
            // 点击卖book
            dAskPrice = price
            nAskVol = 1000

        }

        if(index > 10) {
            // 点击买book
            dBidPrice = price
            nBidVol = 1000

        }
    }

    function increaseBtnClicked() {
        if(0.00 === dBidPrice) {
            infoBar.open(qsTr("请先设置买入价!"));
            return;
        }

        var currStepValue = stepBox.value;
        currStepValue *= 0.01;
        if(0 === currStepValue) {
            infoBar.open(qsTr("价差比率非法!"))
            return;
        }

        dAskPrice = dBidPrice / (1 - currStepValue);
        dAskPrice = Math.ceil(dAskPrice * 100) / 100;

        var rate = (dAskPrice - dBidPrice) / dAskPrice;
        while(rate > 0.05) {
            dAskPrice -= 0.01;
            rate = (dAskPrice - dBidPrice) / dAskPrice;
        }

        if(!checkValid(dAskPrice, dBidPrice)) {
            return;
        }

        nAskVol = nBidVol
    }

    function decreaseBtnClicked() {
        if(0.00 == dAskPrice) {
            infoBar.open(qsTr("请先设置卖出价!"));
            return;
        }

        var currStepValue = stepBox.value;
        currStepValue *= 0.01;
        if(0 === currStepValue) {
            infoBar.open(qsTr("价差比率非法!"))
            return;
        }

        dBidPrice = dAskPrice * (1 - currStepValue);
        dBidPrice = Math.floor(dBidPrice * 100) / 100;

        var rate = (dAskPrice - dBidPrice) / dAskPrice;
        while(rate > 0.05) {
            dBidPrice += 0.01;
            rate = (dAskPrice - dBidPrice) / dAskPrice;
        }


        if(!checkValid(dAskPrice, dBidPrice)) {
            return;
        }
        nBidVol = nAskVol
    }

    function checkValid(ask_price, bid_price) {
        var rate = (ask_price - bid_price) / ask_price;
        if(rate  > 0.05) {
            infoBar.open(qsTr("买卖价差不能超过 %5.00 !"));
            return false;
        }

        if(ask_price  <= bid_price) {
            infoBar.open(qsTr("卖出价不能小于买入价!"));
            return false;
        }
        return true;
    }

    function showOrderInfo(orderInfo) {
        if(null === orderInfo) {
            return;
        }

        if(orderInfo.bAllMatched ){
            reset();
        }

        // 双标报价成功则赋值
        showAskStatus.value = orderInfo.askOrderStatus
        showAskPrice.value = orderInfo.dAskPrice
        showAskWtVol.value = orderInfo.nAskWtVol
        showAskVol.value = orderInfo.nAskVol

        showBidStatus.value = orderInfo.bidOrderStatus
        showBidPrice.value = orderInfo.dBidPrice
        showBidWtVol.value = orderInfo.nBidWtVol
        showBidVol.value = orderInfo.nBidVol
    }

    Component.onCompleted: {
        stockObject.orderInfoChanged.connect(function(orderInfo){
            showOrderInfo(orderInfo);

            infoBar.open(orderInfo.infoBarMsg)
        });

        // 如果委托不为空，显示委托信息
        showOrderInfo(stockObject.orderInfo);
    }

    // 展示区
    Item {
        Layout.preferredWidth: parent.width
        Layout.fillHeight: true

        QC14.GroupBox {
            title: qsTr("当前买卖状态")
            anchors.fill: parent

            ColumnLayout {
                anchors.topMargin: 10
                anchors.fill: parent
                spacing: 5

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
                    Layout.fillHeight: true

                    FLabelValue {
                        id: showAskStatus
                        text: qsTr("叫卖状态: ")
                        value: "- -"
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
                    Layout.fillHeight: true

                    FLabelValue {
                        id: showAskVol
                        text: qsTr("叫卖挂单: ")
                        value: "--"
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
                    Layout.fillHeight: true
                    FLabelValue {
                        id: showAskPrice
                        text: qsTr("叫卖价: ")
                        value: "- -"
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
                    Layout.fillHeight: true
                    FLabelValue {
                        id: showAskWtVol
                        text: qsTr("叫卖数量: ")
                        value: "- -"
                    }
                }


                Item {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
                    Layout.fillHeight: true

                    FLabelValue {
                        id: showBidStatus
                        text: qsTr("叫买状态: ")
                        value: "- -"
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
                    Layout.fillHeight: true
                    FLabelValue {
                        id:  showBidVol
                        text: qsTr("叫买挂单: ")
                        value: "--"
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
                    Layout.fillHeight: true
                    FLabelValue {
                        id: showBidPrice
                        text: qsTr("叫买价: ")
                        value: "- -"
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.maximumWidth: parent.width
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

    Component {
        id: progressBarRun
        ProgressBar {
            from: 0.0
            to: 1.0
            value: 0
            indeterminate: true
        }
    }

    Component {
        id: progressBarStop
        ProgressBar {
            from: 0.0
            to: 1.0
            value: 0
            indeterminate: false
        }
    }

    // 操作区
    Item {
        Layout.preferredWidth: parent.width
        Layout.fillHeight: true

        QC14.GroupBox {
            title: qsTr("手动做市")
            anchors.fill: parent
            checkable: true
            checked: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 5

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true

                    FLabelSpinBox {
                        id: askPrice
                        text: qsTr("卖出价:")
                        decimals: 2
                        stepSize: 0.01
                        value: 0.00
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true

                    FLabelSpinBox {
                        id: askVol
                        text: qsTr("卖出量:")
                        decimals: 0
                        stepSize: 1000
                        minimumValue: 1000
                    }

                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true
                    RowLayout {
                        anchors.fill: parent
                        spacing: 0
                        Rectangle {
                            id: upBtn
                            Layout.preferredWidth: 35
                            Layout.preferredHeight: parent.height
                            Text {
                                anchors.centerIn: parent
                                text: qsTr("∧")
                                font.bold: true
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    increaseBtnClicked()
                                }
                                onEntered: PropertyAnimation {
                                    target: upBtn
                                    properties: "color"
                                    to: "lightGray"
                                    duration: 200
                                }
                                onExited: PropertyAnimation {
                                    target: upBtn
                                    properties: "color"
                                    to: "white"
                                    duration: 200
                                }

                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: parent.height

                            QC14.SpinBox {
                                anchors.fill: parent
                                id: stepBox
                                decimals: 2
                                stepSize: 0.01
                                value: 5.00
                                maximumValue: 5.00
                                minimumValue: 0.01
                                font.pixelSize: 15
                                font.bold: true
                                horizontalAlignment: Qt.AlignHCenter
                            }
                        }

                        Rectangle {
                            id: downBtn
                            Layout.preferredWidth: 35
                            Layout.preferredHeight: parent.height

                            Text {
                                anchors.centerIn: parent
                                text: qsTr("∨")
                                font.bold: true
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    decreaseBtnClicked()
                                }
                                onEntered: PropertyAnimation {
                                    target: downBtn
                                    properties: "color"
                                    to: "lightGray"
                                    duration: 200
                                }
                                onExited: PropertyAnimation {
                                    target: downBtn
                                    properties: "color"
                                    to: "white"
                                    duration: 200
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true

                    FLabelSpinBox {
                        id: bidPrice
                        text: qsTr("买入价:")
                        decimals: 2
                        stepSize: 0.01
                        value: 0.00
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true

                    FLabelSpinBox {
                        id: bidVol
                        text: qsTr("买入量:")
                        decimals: 0
                        stepSize: 1000
                        minimumValue: 1000
                    }

                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true

                    QC14.Button {
                        anchors.fill: parent
                        text: qsTr("双边报价")
                        onClicked: {
                            quote()
                        }
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true

                    RowLayout {
                        anchors.fill: parent
                        spacing: 5

                        QC14.Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: parent.height
                            text: qsTr("撤单")

                            onClicked: {
                                cancelOrder()
                            }
                        }

                        QC14.Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: parent.height
                            text: qsTr("刷新")
                        }
                    }
                }
            }
        }
    }

}

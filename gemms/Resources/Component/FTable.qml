import QtQuick 2.0
import QtQuick.Controls 1.4 as QC14

import qiwookong 1.0

Rectangle {
    id: root

    property alias model: tableView.model
    property alias tableView: tableView
    property alias resources: tableView.resources
    property alias columnComponent: columnComponent

    property int columnCount_: 0
    readonly property alias currentRow: tableView.currentRow

    readonly property color cellBackgroundColor: "#EDEDF0"
    readonly property color cellCurrentRowColor: "#C4DEF4"
    readonly property color cellSelectedColor: "#32A6FF"

    // 右键点击信号
    signal rightBtnClicked(int row)

    // TableView header 代理
    Component {
        id: headerDelegate
        Rectangle {
            width: 50
            height: 30
            radius: 2
            Text {
                id: headerTextInput
                anchors.centerIn: parent
                text: styleData.value
                font.pixelSize: 13
                font.family: "Times"
            }
        }
    }

    // TableView Row 代理
    Component {
        id: rowDelegate
        Rectangle {
            anchors.margins: 3
            width: tableView.width
            height: 25
        }
    }

    // TableView Item 代理
    Component {
        id: itemDelegate

        // Loader 动态加载不同的组件
        Loader {
            id: itemLoader
            anchors.fill: parent
            visible: status == Loader.Ready

            // 根据 role 加载不同的组件 （即根据列名）
            sourceComponent: {
                var role = styleData.role
                //TODO

                return defaultComponent
            }

            //Note: 各种component需要写在loader内部。因为要访问styleData，在外部会
            //提示找不到styleData
            Component {
                id: emptyComponent
                Item { }
            }

            Component {
                id: defaultComponent
                Rectangle {
                    anchors.fill: parent
                    color: isSelected ? cellSelectedColor :
                                        ((tableView.currentRow === styleData.row) ?
                                             cellCurrentRowColor : cellBackgroundColor)

                    property bool isSelected: tableView.currentColumn === styleData.column &&
                                              tableView.currentRow === styleData.row
                    Text {
                        anchors.fill: parent
                        text: styleData.value ? qsTr(String(styleData.value)) : ""
                        font.pixelSize: 15
                        color: parent.isSelected ? "white" : "#1c1d1f"

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        MouseArea {
                            anchors.fill: parent
                            onPressed: {
                                updatePositon()
                            }
                            function updatePositon() {
                                tableView.currentColumn = styleData.column;
                                parent.forceActiveFocus();
                            }
                        }
                    }
                }
            }

        }
    }

    Component {
        id: columnComponent
        QC14.TableViewColumn {}
    }

    QC14.TableView {
        id: tableView
        width: parent.width
        height: parent.height

        alternatingRowColors: true
        backgroundVisible: true

        // 记录点击的 cell
        property int currentColumn: -1

        headerDelegate: BorderImage{
            source: "qrc:/Resources/images/header.png"
            border{left:2;right:2;top:2;bottom:2}
            Text {
                text: styleData.value
                anchors.centerIn:parent
                color:"#333"
                font.pixelSize: 14
                font.family: "华文细黑"
            }
        }
        // 代理
        rowDelegate:  Rectangle {
            height: 25
            Behavior on height{ NumberAnimation{} }

            color: styleData.selected ? "#448" : (styleData.alternate? "#eee" : "#fff")
            BorderImage{
                id: selected
                anchors.fill: parent
                source: "qrc:/Resources/images/selectedrow.png"
                visible: styleData.selected
                border{left:2; right:2; top:2; bottom:2}
                SequentialAnimation {
                    running: true; loops: Animation.Infinite
                    NumberAnimation { target:selected; property: "opacity"; to: 1.0; duration: 900}
                    NumberAnimation { target:selected; property: "opacity"; to: 0.5; duration: 900}
                }
            }
        }

        itemDelegate: Item {
            Text {
                anchors.centerIn: parent
                anchors.verticalCenter: parent.verticalCenter
                color: styleData.textColor
                elide: styleData.elideMode
                text: styleData.value
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                font.pixelSize: 13
                font.family: "华文细黑"
            }
        }

        resources: {
            var roleList = tableView.model.customRoleNames()
            root.columnCount_ = roleList.length
            var temp = []
            for(var i = 0; i < root.columnCount_; ++i)
            {
                var role  = roleList[i]

                var columnWidth = 100;
                switch(role) {
                case qsTr("委托合同序号"):
                    columnWidth = 200;
                    break;
                case qsTr("证券买入成本(实时)"):
                    columnWidth = 150;
                    break;
                case qsTr("今日实际盈亏金额"):
                    columnWidth = 150;
                    break;
                case qsTr("昨日实际盈亏金额"):
                    columnWidth = 150;
                    break;
                case qsTr("今日浮动盈亏金额"):
                    columnWidth = 150;
                    break;
                case qsTr("资金昨日余额"):
                    columnWidth = 180;
                    break;
                case qsTr("资金当前余额"):
                    columnWidth = 180;
                    break;
                case qsTr("资金可用金额"):
                    columnWidth = 180;
                    break;
                case qsTr("资金冻结金额"):
                    columnWidth = 180;
                    break;
                case qsTr("垫付资金"):
                    columnWidth = 180;
                    break;
                case qsTr("资产单元名称"):
                    columnWidth = 150;
                    break;
                default: break;
                }

                var tableViewColumn = {
                    "role": qsTr(role),
                    "title": qsTr(String(role)),
                    "width": columnWidth
                }

                temp.push(columnComponent.createObject(tableView, tableViewColumn))
            }
            return temp
        }

        function resizeColumns() {
            for(var i = 0; i < columnCount; ++i) {
                getColumn(i).resizeToContents()
            }
        }

        onClicked: {
            tableView.currentRow = row
        }
    }

    // 右键菜单
    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true
        acceptedButtons: Qt.RightButton
        onReleased: {
            if (mouse.button === Qt.RightButton) {
                rightBtnClicked(tableView.currentRow)
            }
            mouse.accepted = false
        }
    }

    function resizeAllColumns() {
        tableView.resizeColumnsToContents()
        tableView.resizeColumns()
    }
}

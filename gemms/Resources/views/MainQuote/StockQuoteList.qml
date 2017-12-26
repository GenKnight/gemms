import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Universal 2.0

import QuickFlux 1.0
import Fluid.Controls 1.0

import qiwookong 1.0

import "../../stores"
import "../../actions"
import "../../Component"

ScrollView {
    id: stockQuoteList

    signal chooseStock(string stock)

    ListView {
        id: listView
        anchors.fill: parent
        model: MainStore.stockQuoteListStore.model

        delegate: Component {
            Loader {
                width: parent.width
                sourceComponent:  Component {
                    ListItem {
                        width: parent.width
                        height: 40

                        RowLayout {
                            anchors.fill: parent
                            spacing: 20

                            Item {
                                Layout.preferredWidth : parent.width / 2
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: parent.right
                                    text: model.szStkName
                                    font.family: "黑体"
                                    font.pixelSize: 15
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    text: model.szStkCode
                                    font.family: "黑体"
                                    font.pixelSize: 15
                                }
                            }
                        }

                        onClicked: {
                            chooseStock(model.szStkCode)
                        }
                    }
                }
            }

        }
    }
}

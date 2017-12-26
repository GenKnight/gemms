import QtQuick 2.7
import QtQuick.Layouts 1.3
import Fluid.Controls 1.0
import qiwookong 1.0
import QtQuick.Controls 2.2

Item {
    id: root
    anchors.fill: parent

    property string totalLog_: ""

    ScrollView {
        id: view
        anchors.fill: parent

        TextArea {
            id: logEdit
            anchors.fill: parent
            readOnly: true
            selectByMouse: true
            font.family: "微软雅黑"
            font.pixelSize: 14
        }
    }

    function preAppend(log) {
        totalLog_ = log + "\n" + totalLog_;
        logEdit.text = totalLog_;
    }
}

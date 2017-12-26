import QtQuick 2.0
import QtQuick.Controls.Styles 1.4

TabViewStyle {
    frameOverlap: 1
    tab: Rectangle {
        color: styleData.selected ? "steelblue" :"lightsteelblue"
        border.color:  "lightsteelblue" //"steelblue"
        implicitWidth: Math.max(text.width + 4, 80)
        implicitHeight: 20
        radius: 2
        Text {
            id: text
            anchors.centerIn: parent
            text: styleData.title
            color: styleData.selected ? "white" : "black"
            font.pixelSize: 15
            font.family: "黑体"
        }
    }
   frame: Item {  }
}

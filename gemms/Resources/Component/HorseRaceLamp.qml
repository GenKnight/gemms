import QtQuick 2.0

Rectangle{
    id:rectText
    x:100;y:200
    width: 300;height: 20
    border.color: "blue"
    border.width: 2
    clip:true

    Text{
        id:contentTextId

        text:"dsfagdfsddddddnvhfghddddsdddddfgjhkmukhdddjhjhggdddddfgdfhdfyjfugkybnsfgbsrgrt"
        font.pointSize: 10

        NumberAnimation {
            id:animText
            target: contentTextId
            property: "x"
            duration: 15000
            to:(contentTextId.text.length - rectText.width )
            running: false
            easing.type: Easing.OutCubic
        }

    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            animText.start()
        }
    }
}

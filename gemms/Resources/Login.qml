import QtQuick 2.6
import QtQuick.Window 2.3
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import Fluid.Controls 1.0

import QtQuick.Controls 1.4 as QC14

import qiwookong 1.0
import Qt.labs.settings 1.0 as QS

Window {
    id: root
    objectName: "loginWindow"
    width: 715
    height: 455
    minimumHeight: height
    minimumWidth: width
    maximumHeight: height
    maximumWidth: width
    visible: true

    property bool tdLoginSuccess: false
    property int rowSpacing: 15
    property int fontSize: 15

    ColumnLayout {
        width: parent.width / 2
        height: parent.height - 40
        anchors.centerIn: parent
        opacity: 0

        Item {
            Layout.preferredWidth: 256
            Layout.preferredHeight: 256
            anchors.horizontalCenter: parent.horizontalCenter
            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                antialiasing: true
                source: "qrc:/Resources/images/cefc.png"

                RotationAnimation on rotation {
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 60000
                }
            }
        }

        Item {
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 30
            Label {
                id: label
                font.family: " 黑体"
                font.weight: Font.Bold
                font.pixelSize: 30
                text: qsTr("华信自动化做市交易系统")
            }
        }

        Item {
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 80

            ColumnLayout {
                anchors.fill: parent

                Row {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true
                    spacing: rowSpacing

                    Label {
                        id: brokerIDLabel
                        width: 100
                        font.family: "Roboto"
                        font.weight: Font.Bold
                        text: qsTr("用户名")
                        font.pixelSize: fontSize
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TextField {
                        id: userNameField
                        width: parent.width - brokerIDLabel.width - parent.spacing
                        placeholderText: qsTr("用户名")
                    }
                }

                Row {
                    Layout.preferredWidth: parent.width
                    Layout.fillHeight: true
                    spacing: rowSpacing

                    Label {
                        id: passwordLabel
                        width: 100
                        font.family: "Roboto"
                        font.weight: Font.Bold
                        text: qsTr("交易密码")
                        font.pixelSize: fontSize
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TextField {
                        id: passwordField
                        echoMode: TextInput.Password
                        width: parent.width - passwordLabel.width - parent.spacing - remberCheckbox.implicitWidth
                        onAccepted: {
                            if(passwordField.text.length != 6 || !checkRate(passwordField.text)) {
                                label.text = qsTr("密码格式不对")
                            } else {
                                login()
                            }
                        }
                    }

                    CheckBox {
                        id: remberCheckbox
                        anchors.verticalCenter: parent.verticalCenter
                        checked: true

                        onClicked: {
                            passwordField.forceActiveFocus();
                        }
                    }
                }

//                Row {
//                    Layout.preferredWidth: parent.width
//                    Layout.fillHeight: true

//                    CheckBox {
//                        id: remberCheckbox
//                        anchors.verticalCenter: parent.verticalCenter
//                        checked: true
//                        text: qsTr("记住密码")

//                        onClicked: {
//                            passwordField.forceActiveFocus();
//                        }
//                    }
//                }
            }
        }

        NumberAnimation on opacity {
            id: fadeIn
            to: 1.0
            duration: 3000
        }

        Component.onCompleted: {
            fadeIn.start()
        }
    }

    QS.Settings {
        id: settings
        property bool isRember: true
        property string userName: ""
        property string password: ""
    }

    // 用户登陆接口
    UserApi {
        id: userApi

        onLoginStatusChanged: {
            console.log(msg)
            label.text = String(msg);
        }
    }

    function login() {
        if("" === userNameField.text) {
            label.text = qsTr("请输入用户名")
            return
        }

        if ("" == passwordField.text) {
            label.text = qsTr("请输入交易密码")
            return
        }

        label.text = qsTr("Please wait...")
        disableEdit()

        remberPassword();
        userApi.userReqLogin(userNameField.text, passwordField.text);
    }

    function disableEdit() {
        userNameField.enabled = false
        passwordField.enabled = false
    }

    function enableEdit() {
        userNameField.enabled = true
        passwordField.enabled = true
    }

    function checkRate(nubmer) {
        var re = /^[0-9]+.?[0-9]*$/; //判断字符串是否为数字 //判断正整数 /^[1-9]+[0-9]*]*$/
        if (!re.test(nubmer)) {
            return false;
        }
        return true
    }

    function remberPassword() {
        // 记住密码
        settings.userName = userNameField.text
        settings.isRember = remberCheckbox.checked
        if(settings.isRember) {
            settings.password = passwordField.text
        }
    }

    Component.onCompleted: {
        var remberd = settings.isRember
        var userName = settings.userName
        userNameField.forceActiveFocus();
        if(userName !== "" && userName !== "undefined") {
            userNameField.text = userName
            passwordField.forceActiveFocus()
        }

        remberCheckbox.checked = settings.isRember;
        if(settings.isRember) {
            var password = settings.password;
            if(password !== "" && password !== "undefined") {
                passwordField.text = password
            }
        }
    }
}

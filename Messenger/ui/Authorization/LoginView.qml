import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import P2PMessenger 1.0

Rectangle{
    anchors.fill: parent;
    color: '#313131'
    id: contacts


    Rectangle {

        width: 160
        height: 150
        border.color: '#000000'
        border.width: 1
        anchors.centerIn: parent
        color: '#656565'


        Column {

            spacing: 3

            anchors.centerIn: parent

            TextField {
                id: loginInput

                anchors.horizontalCenter: parent.horizontalCenter
                echoMode: TextInput.Normal
                height: 20
                width: 140
            }

            TextField {
                id: passwordInput

                anchors.horizontalCenter: parent.horizontalCenter
                echoMode: TextInput.Password
                height: 20
                width: 140
            }

            Button {
                id: submit

                anchors.horizontalCenter: parent.horizontalCenter
                height: 30
                text: "Log in"
                width: 140

                onClicked: {
                    authorizationHandler.login(loginInput.text, passwordInput.text);
                }
            }
        }
    }
}

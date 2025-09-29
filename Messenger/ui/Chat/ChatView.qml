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

        }
    }
}
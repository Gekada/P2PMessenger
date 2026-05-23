import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    width: 900
    height: 600
    visible: true
    title: "P2P Messenger"
    color: "#1E1E1E"

    Component.onCompleted: chatController.loadContacts()

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // LEFT PANEL: Contacts
        Rectangle {
            Layout.preferredWidth: 280 // Slightly wider for the Hex ID
            Layout.fillHeight: true
            color: "#252526"

            // Everything inside the left panel stacks vertically here
            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // 1. My Peer ID Section (Top)
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 65
                    color: "#1E1E1E"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 4

                        Label {
                            text: "My Peer ID"
                            color: "#888888"
                            font.pixelSize: 11
                            font.bold: true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 5

                            TextField {
                                id: myIdField
                                Layout.fillWidth: true
                                text: chatController.myPeerId
                                color: "#0A84FF"
                                font.pixelSize: 11
                                readOnly: true
                                selectByMouse: true
                                background: Rectangle {
                                    color: "transparent"
                                }
                                leftPadding: 0
                            }

                            Button {
                                id: copyBtn
                                text: "Copy"
                                Layout.preferredHeight: 25
                                Layout.preferredWidth: 50

                                contentItem: Text {
                                    text: parent.text
                                    font.pixelSize: 11
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                background: Rectangle {
                                    color: "#333333"; radius: 4
                                }

                                onClicked: {
                                    myIdField.selectAll()
                                    myIdField.copy()
                                    myIdField.deselect()

                                    text = "Copied!"
                                    copyTimer.start()
                                }

                                Timer {
                                    id: copyTimer
                                    interval: 2000
                                    onTriggered: copyBtn.text = "Copy"
                                }
                            }
                        }
                    }
                    // Bottom Separator
                    Rectangle {
                        width: parent.width; height: 1; anchors.bottom: parent.bottom; color: "#333333"
                    }
                }

                // 2. Start New Chat Input
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    color: "#252526"

                    TextField {
                        anchors.centerIn: parent
                        width: parent.width - 20
                        placeholderText: "Start new chat (Peer ID)"
                        color: "white"
                        background: Rectangle {
                            color: "#333333"; radius: 4
                        }
                        onAccepted: {
                            chatController.selectChat(text)
                            text = ""
                        }
                    }
                    // Bottom Separator
                    Rectangle {
                        width: parent.width; height: 1; anchors.bottom: parent.bottom; color: "#333333"
                    }
                }

                // 3. Contacts List
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: chatController.contacts
                    clip: true

                    delegate: Rectangle {
                        width: parent.width
                        height: 60
                        color: chatController.currentPeerId === modelData.peerId ? "#3A3A3C" : "transparent"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: chatController.selectChat(modelData.peerId)
                        }

                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 15

                            Text {
                                text: modelData.peerId.substring(0, 15) + "..."
                                color: "white"
                                font.pixelSize: 14
                                font.bold: true
                            }
                        }

                        Rectangle {
                            width: parent.width; height: 1
                            anchors.bottom: parent.bottom
                            color: "#333333"
                        }
                    }
                }
            }

            // Right border separating left panel from chat
            Rectangle {
                width: 1; anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom; color: "#333333"
            }
        }

        // RIGHT PANEL: Active Chat
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                color: "#1E1E1E"

                Label {
                    anchors.centerIn: parent
                    text: chatController.currentPeerId === "" ? "Select a Peer" : "Chatting with: " + chatController.currentPeerId.substring(0, 8) + "..."
                    color: "#FFFFFF"
                    font.pixelSize: 16
                }
                Rectangle { width: parent.width; height: 1; anchors.bottom: parent.bottom; color: "#333333" }
            }

            ListView {
                id: messageListView
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: chatController.currentMessages // Bind to DB messages!
                clip: true
                spacing: 10
                topMargin: 20; bottomMargin: 20; leftMargin: 20; rightMargin: 20

                delegate: Item {
                    width: ListView.view.width - 40
                    height: bubble.height

                    Rectangle {
                        id: bubble
                        // Note the modelData prefix!
                        anchors.right: modelData.isMine ? parent.right : undefined
                        anchors.left: modelData.isMine ? undefined : parent.left

                        width: Math.min(messageText.implicitWidth + 24, parent.width * 0.75)
                        height: messageText.implicitHeight + 16
                        radius: 8
                        color: modelData.isMine ? "#0A84FF" : "#3A3A3C"

                        Text {
                            id: messageText
                            text: modelData.message
                            color: "white"
                            anchors.centerIn: parent
                            width: parent.width - 24
                            wrapMode: Text.Wrap
                        }
                    }
                }
                onCountChanged: messageListView.positionViewAtEnd()
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 70
                color: "#252526"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10

                    TextField {
                        id: messageInput
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        placeholderText: "Type a message..."
                        color: "white"
                        background: Rectangle { color: "#333333"; radius: 20 }
                        leftPadding: 15
                        onAccepted: sendBtn.clicked()
                    }

                    Button {
                        id: sendBtn
                        text: "Send"
                        enabled: messageInput.text.trim() !== "" && chatController.currentPeerId !== ""
                        onClicked: {
                            chatController.sendMessage(messageInput.text)
                            messageInput.text = ""
                        }
                    }
                }
            }
        }
    }
}
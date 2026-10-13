import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import VOX_Client

Item
{
    id: chatWorkSpaceRoot
    anchors.fill: parent

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        // ШАПКА ЧАТА
        Rectangle
        {
            Layout.fillWidth: true; Layout.preferredHeight: 48; color: "#313338"
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: "#1f2023" }
            RowLayout
            {
                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 8
                Text { text: "#"; color: "#80848e"; font.pixelSize: 20; font.bold: true }
                Text { text: "Текстовый канал"; color: "#ffffff"; font.bold: true; font.pixelSize: 15 }
                Rectangle { width: 1; height: 16; color: "#3f4147"; Layout.leftMargin: 8; Layout.rightMargin: 8 }
                Text { text: "Добро пожаловать в VOX"; color: "#949ba4"; font.pixelSize: 12; Layout.fillWidth: true; elide: Text.ElideRight }
            }
        }

        // ЛЕНТА СООБЩЕНИЙ
        MessageList
        {
            Layout.fillWidth: true; Layout.fillHeight: true
        }

        // ПОЛЕ ВВОДА
        Rectangle
        {
            Layout.fillWidth: true; Layout.preferredHeight: 68; color: "#313338"
            Rectangle
            {
                anchors.fill: parent; anchors.margins: 16; color: "#383a40"; radius: 8
                RowLayout
                {
                    anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 12
                    TextField
                    {
                        id: messageInputField
                        Layout.fillWidth: true; placeholderText: "Написать сообщение..."
                        placeholderTextColor: "#646970"; color: "#dbdee1"; font.pixelSize: 14
                        background: Item {}
                        onAccepted: {
                            if (text.trim() !== "") {
                                ChatManager.sendMessageFromUI(text.trim());
                                text = ""
                            }
                        }
                    }
                    Button
                    {
                        implicitWidth: 32; implicitHeight: 32; flat: true
                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                        background: Rectangle { color: parent.hovered ? "#4752c4" : "#5865f2"; radius: 4 }
                        contentItem: Text { text: "➔"; color: "white"; font.bold: true; anchors.centerIn: parent }
                        onClicked: messageInputField.accepted()
                    }
                }
            }
        }
    }
}

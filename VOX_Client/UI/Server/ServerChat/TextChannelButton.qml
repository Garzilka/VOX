import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ItemDelegate {
    id: control
    width: parent.width
    height: 34

    property string channelGUID: ""
    property string channelName: ""

    readonly property bool isSelected: ChatManager.activeRoomGUID === channelGUID

    background: Rectangle {
        color: control.isSelected ? "#404249" : (control.hovered ? "#35373c" : "transparent")
        radius: 4
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
    }

    contentItem: RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 6

        Text {
            text: "#"
            color: control.isSelected ? "#ffffff" : (control.hovered ? "#dbdee1" : "#949ba4")
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            text: control.channelName ? control.channelName : "без названия"
            color: control.isSelected ? "#ffffff" : (control.hovered ? "#dbdee1" : "#949ba4")
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 13
            font.bold: control.isSelected
            Layout.fillWidth: true
            elide: Text.ElideRight
        }
    }

    onClicked: {
        ChatManager.activeRoomGUID = channelGUID
    }
}

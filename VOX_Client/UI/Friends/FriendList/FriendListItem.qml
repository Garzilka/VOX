import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ItemDelegate
{
    id: friendRowRoot

    // Входные роли модели
    property var _avatarColor
    property var _statusColor
    property var _name
    property var _status
    property var _currentActivity

    width: parent ? parent.width : 200
    height: 60

    background: Rectangle
    {
        color: friendRowRoot.hovered ? "#35373c" : "transparent"
        radius: 8
    }

    TapHandler
    {
        acceptedButtons: Qt.LeftButton
        onTapped:
        {
            // modelData — это напрямую строка UUID
            ChatManager.openOrCreateChatFromUI(0, modelData, "", "");
        }
    }

    signal rightClicked(string friendName, string friendGUID, Item sourceItem, real mouseX, real mouseY)

    TapHandler
    {
        id: rightTap
        acceptedButtons: Qt.RightButton
        onTapped: friendRowRoot.rightClicked(_name, modelData, friendRowRoot, rightTap.point.position.x, rightTap.point.position.y)
    }

    // Разделяющая линия
    Rectangle
    {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        height: 1
        color: "#3f4147"
        visible: !friendRowRoot.hovered
    }

    // ==========================================
    // Основная разметка (Содержимое карточки)
    // ==========================================
    RowLayout
    {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 12

        // ==========================================
        // Аватар
        // ==========================================
        Rectangle
        {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            radius: 20
            color: _avatarColor

            // ==========================================
            // Точка статуса
            // ==========================================
            Rectangle
            {
                width: 12
                height: 12
                radius: 6
                color: _statusColor
                border.color: friendRowRoot.hovered ? "#35373c" : "#313338"
                border.width: 2
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }

        // ==========================================
        // Никнейм и статус-активность
        // ==========================================
        ColumnLayout
        {
            spacing: 2
            Layout.fillWidth: true

            Text
            {
                text: _name
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 14
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Text
            {
                text: _currentActivity !== "" ? "Занят: " + _currentActivity : _status
                color: "#b5bac1"
                font.pixelSize: 12
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }
    }
}

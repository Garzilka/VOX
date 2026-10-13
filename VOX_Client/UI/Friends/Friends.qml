import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Item
{
    anchors.fill: parent

    RowLayout
    {
        anchors.fill: parent
        spacing: 0

        // ==========================================
        // Боковая панель друзей
        // ==========================================
        FriendsPannel
        {
            Layout.fillHeight: true
            Layout.fillWidth: false
            Layout.preferredWidth: 240
            Layout.minimumWidth: 240
            Layout.maximumWidth: 240
        }

        // ==========================================
        // Вертикальный разделитель (тонкая линия)
        // ==========================================
        Rectangle
        {
            color: "#1f2023"
            Layout.fillHeight: true
            implicitWidth: 1
        }

        // ==========================================
        // Основная рабочая область (список контактов / чат)
        // ==========================================
        FriendsWorkSpace
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}

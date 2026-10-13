import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import VOX_Client

Item
{
    anchors.fill: parent
    Layout.fillWidth: true
    Layout.fillHeight: true

    ListModel
    {
        id: serverMessagesModel
    }

    StackLayout
    {
        id: mainStackLayout
        anchors.fill: parent
        currentIndex: ServerManager.State


        // ==========================================
        // Слой 0: Интерфейс Чата
        // ==========================================
        Rectangle
        {
            color: "#313338"
            Layout.fillWidth: true
            Layout.fillHeight: true

            ChatSpace
            {
                anchors.fill: parent
            }
        }

        // ==========================================
        // Слой 1: Интерфейс Канбан-Доски
        // ==========================================
        Rectangle
        {
            color: "#313338"
            Layout.fillWidth: true
            Layout.fillHeight: true
            KanBan
            {
                anchors.fill: parent
            }
        }

        // ==========================================
        // Слой 2: Текстовый редактор / Документы
        // ==========================================
        Rectangle
        {
            color: "#313338"
            Layout.fillWidth: true
            Layout.fillHeight: true
            DocumentEditor
            {
                anchors.fill: parent
            }
        }
    }
}

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import VOX_Client

Item
{
    anchors.fill: parent
    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        Rectangle
        {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: "#232428" // Выдение темным тоном, как шапку

            RowLayout
            {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 4

                // ==========================================
                // Кнопка: Чат
                // ==========================================
                Button
                {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    flat: true
                    background: Rectangle
                    {
                        color: ServerManager.State === 0 ? "#35373c" : "transparent"
                        radius: 4
                    }
                    text: "💬"
                    onClicked: ServerManager.State = 0
                    ToolTip.visible: hovered
                    ToolTip.text: "Чаты"
                }

                // ==========================================
                // Кнопка: Доски
                // ==========================================
                Button
                {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    flat: true
                    background: Rectangle
                    {
                        color: ServerManager.State === 1 ? "#35373c" : "transparent"
                        radius: 4
                    }
                    text: "📋"
                    onClicked: ServerManager.State = 1
                    ToolTip.visible: hovered
                    ToolTip.text: "Доски задач"
                }

                // ==========================================
                // Кнопка: Документы
                // ==========================================
                Button
                {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    flat: true
                    background: Rectangle
                    {
                        color: ServerManager.State === 2 ? "#35373c" : "transparent"
                        radius: 4
                    }
                    text: "📄"
                    onClicked: ServerManager.State = 2
                    ToolTip.visible: hovered
                    ToolTip.text: "База знаний"
                }
            }
        }

        // Разделительная линия под таб-баром
        Rectangle
        {
            Layout.fillWidth: true
            height: 1
            color: "#1f2023"
        }

        StackLayout
        {
            id: tabSwitcher
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: ServerManager.State

            // ==========================================
            // Слой 0: Интерфейс Чата
            // ==========================================
            Rectangle
            {
                color: "#313338"

                Chats
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
                Boards
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
                Documents
                {
                    anchors.fill: parent
                }
            }
        }
    }
}

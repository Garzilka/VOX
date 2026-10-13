import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item
{
    id: leftPanelRoot
    anchors.fill: parent

    ColumnLayout
    {
        anchors.fill: parent
        anchors.topMargin: 12
        spacing: 12

        // ==========================================
        // ВЕРХНЯЯ КНОПКА: ЛИЧНОЕ ПРОСТРАНСТВО (ДРУЗЬЯ)
        // ==========================================
        Button
        {
            id: personalButton
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 48
            implicitHeight: 48
            flat: true

            HoverHandler
            {
                id: personalHover;
                cursorShape: Qt.PointingHandCursor
            }

            background: Rectangle
            {
                color: UIStateManager.WorkState === 0 ? "#5865f2" : (personalButton.hovered ? "#35373c" : "#313338")
                radius: UIStateManager.WorkState === 0 ? 16 : (personalButton.hovered ? 16 : 24)

                Behavior on radius { NumberAnimation { duration: 100 } }
                Behavior on color { ColorAnimation { duration: 100 } }
            }

            contentItem: Text
            {
                text: "💼"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked:
            {
                ServerManager.activeServerGUID = ""
                UIStateManager.openPersonalWorkspace()
            }

            ToolTip.visible: hovered
            ToolTip.text: "Личные сообщения и друзья"
        }

        // Разделительная линия
        Rectangle
        {
            Layout.alignment: Qt.AlignHCenter
            width: 32
            height: 2
            color: "#35363c"
        }

        // ==========================================
        // СПИСОК РЕАЛЬНЫХ СЕРВЕРОВ ИЗ C++ CATCH
        // ==========================================
        Repeater
        {
            model: ServerManager.serverList

            delegate: Button
            {
                id: serverButton
                Layout.alignment: Qt.AlignHCenter
                implicitWidth: 48
                implicitHeight: 48
                flat: true

                HoverHandler { cursorShape: Qt.PointingHandCursor }

                readonly property bool isActive: ServerManager.activeServerGUID === modelData.serverGUID

                background: Rectangle
                {
                    color: serverButton.isActive ? "#5865f2" : (serverButton.hovered ? "#35373c" : "#313338")
                    radius: (serverButton.hovered || serverButton.isActive) ? 16 : 24

                    Behavior on radius { NumberAnimation { duration: 100 } }
                    Behavior on color { ColorAnimation { duration: 100 } }
                }

                contentItem: Text
                {
                    text: modelData.name ? modelData.name.substring(0, 2).toUpperCase() : "?"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked:
                {
                    UIStateManager.WorkState = 1
                    ServerManager.setActiveServerGUID(modelData.serverGUID)
                }

                ToolTip.visible: hovered
                ToolTip.text: modelData.name
            }
        }

        // ==========================================
        // КНОПКА "+": СОЗДАТЬ СЕРВЕР
        // ==========================================
        Button
        {
            id: createServerButton
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 48
            implicitHeight: 48
            flat: true

            HoverHandler { cursorShape: Qt.PointingHandCursor }

            background: Rectangle
            {
                color: createServerButton.hovered ? "#23a55a" : "#313338"
                radius: createServerButton.hovered ? 16 : 24

                Behavior on radius { NumberAnimation { duration: 100 } }
                Behavior on color { ColorAnimation { duration: 100 } }
            }

            contentItem: Text
            {
                text: "+"
                color: createServerButton.hovered ? "#ffffff" : "#23a55a"
                font.bold: true
                font.pixelSize: 22
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                Behavior on color { ColorAnimation { duration: 100 } }
            }

            // ИСПРАВЛЕНИЕ: Открываем диалог через общий менеджер состояний интерфейса
            onClicked: UIStateManager.isCreateServerDialogOpen = true

            ToolTip.visible: hovered
            ToolTip.text: "Добавить сервер"
        }

        Item
        {
            Layout.fillHeight: true
        }
    }
}

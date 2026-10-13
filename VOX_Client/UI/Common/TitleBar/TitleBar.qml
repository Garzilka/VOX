import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle
{
    id: titleBarRoot
    height: 24
    color: "#1e1f22"
    property var mainAppWindow: null

    RowLayout
    {
        anchors.fill: parent
        spacing: 0

        // ==========================================
        // Левая часть: Иконка и название приложения
        // ==========================================
        RowLayout
        {
            Layout.leftMargin: 12
            spacing: 8

            // ==========================================
            // Логотип
            // ==========================================
            Text
            {
                text: "🚀"
                font.pixelSize: 16
            }

            Text
            {
                text: "VOX"
                color: "#949ba4"
                font.bold: true
                font.pixelSize: 12
            }
        }

        // ==========================================
        // Центральная пустая область, за которую можно перетаскивать окно
        // ==========================================
        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

            MouseArea
            {
                anchors.fill: parent
                onPressed:
                {
                    if (titleBarRoot.mainAppWindow)
                    {
                        titleBarRoot.mainAppWindow.startSystemMove();
                    }
                }

                onDoubleClicked:
                {
                    if (!titleBarRoot.mainAppWindow) return;

                    if (titleBarRoot.mainAppWindow.visibility === Window.Maximized)
                        titleBarRoot.mainAppWindow.showNormal();
                    else
                        titleBarRoot.mainAppWindow.showMaximized();
                }
            }
        }

        // ==========================================
        // Правая часть: Системные кнопки (Свернуть, Развернуть, Закрыть)
        // ==========================================
        RowLayout
        {
            spacing: 0
            Layout.fillHeight: true

            // Кнопка СВЕРНУТЬ
            Button
            {
                id: minimizeBtn
                Layout.fillHeight: true
                implicitWidth: 45

                background: Rectangle
                {
                    color: minimizeBtn.hovered ? "#35373c" : "transparent"
                }
                contentItem: Text
                {
                    text: "—"
                    color: "#dbdee1"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                }
                onClicked: Window.window.showMinimized()
            }

            // ==========================================
            // Кнопка РАЗВЕРНУТЬ / СВЕРНУТЬ В ОКНО
            // ==========================================
            Button
            {
                id: maximizeBtn
                Layout.fillHeight: true
                implicitWidth: 45

                background: Rectangle
                {
                    color: maximizeBtn.hovered ? "#35373c" : "transparent"
                }
                contentItem: Text
                {
                    // Меняем иконку в зависимости от состояния окна
                    text: Window.window.visibility === Window.Maximized ? "🗗" : "🗖"
                    color: "#dbdee1"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                }
                onClicked:
                {
                    if (Window.window.visibility === Window.Maximized)
                        Window.window.showNormal();
                    else
                        Window.window.showMaximized();
                }
            }

            // ==========================================
            // Кнопка ЗАКРЫТЬ
            // ==========================================
            Button
            {
                id: closeBtn
                Layout.fillHeight: true
                implicitWidth: 45

                background: Rectangle
                {
                    color: closeBtn.hovered ? "#f23f43" : "transparent"
                }
                contentItem: Text
                {
                    text: "✕"
                    color: closeBtn.hovered ? "#ffffff" : "#dbdee1"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12
                }
                onClicked: Window.window.close()
            }
        }
    }

    // ==========================================
    // Тонкая разделяющая линия снизу шапки
    // ==========================================
    Rectangle
    {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#1f2023"
    }
}

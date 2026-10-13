import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item
{
    id: settingsOverlayRoot
    anchors.fill: parent

    Rectangle
    {
        anchors.fill: parent
        color: "#313338"
    }

    TapHandler
    {
        cursorShape: Qt.ArrowCursor
    }
    RowLayout
    {
        anchors.fill: parent
        spacing: 0

        // ==========================================
        // Меню
        // ==========================================
        Rectangle
        {
            Layout.fillHeight: true
            Layout.minimumWidth: 200
            Layout.maximumWidth: 400
            color: "#2b2d31"

            ColumnLayout
            {
                anchors.fill: parent
                anchors.topMargin: 60
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                spacing: 4

                Text
                {
                    text: "НАСТРОЙКИ ПРИЛОЖЕНИЯ"
                    color: "#949ba4"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.leftMargin: 10
                    Layout.bottomMargin: 6
                }

                Button
                {
                    id: voiceMenuBtn
                    Layout.fillWidth: true
                    implicitHeight: 32
                    leftPadding: 10
                    HoverHandler { cursorShape: Qt.PointingHandCursor }

                    background: Rectangle
                    {
                        color: voiceMenuBtn.hovered ? "#35373c" : "#404249"
                        radius: 4
                    }

                    contentItem: Text
                    {
                        text: "Голос и видео"
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Item { Layout.fillHeight: true } // Выталкивает меню наверх
            }
        }

        // Вертикальный разделитель между Левым меню и Центром
        Rectangle {
            Layout.fillHeight: true
            implicitWidth: 1
            color: "#3f4147"
        }

        // ==========================================
        // ЦЕНТРАЛЬНЫЙ БЛОК
        // ==========================================
        Rectangle
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#313338"

            RowLayout
            {
                anchors.fill: parent
                spacing: 0

                // Чтобы весь контент плавно начинался ниже верхнего края экрана
                anchors.topMargin: 30

                // Левая пружина: забирает всё свободное пространство слева
                Item
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                // Сам блок настроек с ограничением в 700 пикселей
                Item
                {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.minimumWidth: 460
                    Layout.preferredWidth: 700
                    Layout.maximumWidth: 700

                    InputSettings {}
                }

                // Правая пружина: забирает всё свободное пространство справа
                Item
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        // Вертикальный разделитель между Центром и Кнопкой ESC
        Rectangle {
            Layout.fillHeight: true
            implicitWidth: 1
            color: "#3f4147"
        }

        // ==========================================
        // Кнопка ESC
        // ==========================================
        Item
        {
            Layout.fillHeight: true
            Layout.minimumWidth: 70     // Ваше условие: фиксированный 100px
            Layout.maximumWidth: 70

            ColumnLayout
            {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 60
                spacing: 4
                width: 40

                Button
                {
                    id: closeSettingsBtn
                    implicitWidth: 36
                    implicitHeight: 36
                    Layout.alignment: Qt.AlignHCenter
                    HoverHandler { cursorShape: Qt.PointingHandCursor }

                    background: Rectangle
                    {
                        color: "transparent"
                        radius: 18
                        border.color: closeSettingsBtn.hovered ? "#f23f43" : "#b5bac1"
                        border.width: 1
                    }

                    contentItem: Text
                    {
                        text: "✕"
                        color: closeSettingsBtn.hovered ? "#f23f43" : "#b5bac1"
                        font.pixelSize: 16
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: closeAction()
                }

                Text
                {
                    text: "ESC"
                    color: closeSettingsBtn.hovered ? "#f23f43" : "#b5bac1"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    function closeAction() {
        if (typeof ServerManager !== 'undefined' && ServerManager.activeServerGUID !== "") {
            UIStateManager.MainUIState = 1
        } else if (typeof UIStateManager !== 'undefined') {
            UIStateManager.openPersonalWorkspace()
        }
    }

    focus: true
    Keys.onPressed: (event) =>
    {
        if (event.key === Qt.Key_Escape)
        {
            closeAction()
            event.accepted = true
        }
    }
}

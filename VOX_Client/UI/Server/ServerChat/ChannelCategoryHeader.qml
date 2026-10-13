import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item {
    id: headerRoot
    width: parent.width
    height: 32

    // Входные свойства
    property string title: ""
    property bool isCollapsed: false // Состояние категории (свернута/развернута)

    // Сигналы для внешних обработчиков
    signal toggleCollapsed()
    signal addChannelClicked()

    // Область наведения на весь заголовок, чтобы показывать кнопку "+"
    HoverHandler {
        id: headerHover
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 4

        // Интерактивная зона для стрелочки и текста (клик сворачивает/разворачивает)
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 6

            Text {
                text: "›"
                color: headerHover.hovered ? "#dbdee1" : "#949ba4"
                font.pixelSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                // Вращаем стрелочку на 90 градусов, если категория открыта
                transform: Rotation {
                    origin.x: 4; origin.y: 8
                    angle: headerRoot.isCollapsed ? 0 : 90
                    Behavior on angle { NumberAnimation { duration: 150 } }
                }
            }

            Text {
                text: headerRoot.title.toUpperCase()
                color: headerHover.hovered ? "#dbdee1" : "#949ba4"
                font.pixelSize: 11
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            TapHandler {
                onTapped: headerRoot.toggleCollapsed()
                cursorShape: Qt.PointingHandCursor
            }
        }

        // Кнопка "+" для создания нового канала в этой категории
        Button {
            id: addChannelButton
            implicitWidth: 18
            implicitHeight: 18
            flat: true

            // Кнопка плавно появляется только при наведении на весь заголовок
            opacity: headerHover.hovered ? 1.0 : 0.0
            visible: opacity > 0.0
            Behavior on opacity { NumberAnimation { duration: 100 } }

            HoverHandler { id: btnHover; cursorShape: Qt.PointingHandCursor }

            contentItem: Text {
                text: "+"
                color: btnHover.hovered ? "#dbdee1" : "#949ba4"
                font.pixelSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle { color: "transparent" }

            onClicked: headerRoot.addChannelClicked()
        }
    }
}

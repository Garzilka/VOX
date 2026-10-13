import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ColumnLayout
{
    id: keybindRoot
    Layout.fillWidth: true
    spacing: 0 // Обнуляем spacing, так как используем точные margin для полосы

    // Входное свойство для названия настройки
    property string settingName: ""

    // Свойства для внешнего доступа к сохраненной клавише
    property string currentKey: "Не назначено"
    property bool isRecording: false

    // Строка: Текст слева, Кнопка справа
    RowLayout
    {
        Layout.fillWidth: true
        spacing: 20

        // Текст (без лишних Item-оберток)
        Text
        {
            text: keybindRoot.settingName
            color: "white"
            font.pixelSize: 14
            font.bold: true

            // Занимает левую половину, выравнивается по центру высоты строки
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            wrapMode: Text.Wrap
        }

        // Кнопка (без лишних Item-оберток)
        Button
        {
            id: keybindButton

            // Задаем гибкие, но контролируемые размеры для правого блока
            Layout.minimumWidth: 180
            Layout.preferredWidth: 240
            Layout.maximumWidth: 320
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter // Центрируем кнопку вертикально относительно текста

            implicitHeight: 40
            leftPadding: 12
            rightPadding: 12

            focusPolicy: Qt.ClickFocus

            HoverHandler {
                cursorShape: Qt.PointingHandCursor
            }

            background: Rectangle {
                color: "#1e1f22"
                radius: 4
                border.color: keybindRoot.isRecording ? "#23a55a" : (keybindButton.activeFocus ? "#5865f2" : "#111214")
                border.width: 1
            }

            contentItem: RowLayout
            {
                anchors.fill: parent
                spacing: 0

                // Переносим внутренние отступы прямо в макет текста
                Layout.leftMargin: 12
                Layout.rightMargin: 12

                Text
                {
                    Layout.leftMargin: 12
                    text: keybindRoot.isRecording ? "Запись... Нажмите клавишу" : keybindRoot.currentKey
                    color: keybindRoot.isRecording ? "#23a55a" : "white"
                    font.pixelSize: 14
                    font.bold: !keybindRoot.isRecording
                    Layout.fillWidth: true
                    verticalAlignment: Text.AlignVCenter
                }

                Text
                {
                    text: keybindRoot.isRecording ? "ESC для отмены" : "Изменить"
                    color: "#b5bac1"
                    font.pixelSize: 12
                    verticalAlignment: Text.AlignVCenter
                    Layout.rightMargin: 12
                }
            }

            onClicked: {
                keybindRoot.isRecording = true
            }

            Keys.onPressed: (event) => {
                if (!keybindRoot.isRecording) return;

                if (event.key === Qt.Key_Escape) {
                    keybindRoot.isRecording = false;
                    event.accepted = true;
                    return;
                }

                keybindRoot.currentKey = event.text ? event.text.toUpperCase() : "Key_" + event.key;
                keybindRoot.isRecording = false;
                keybindButton.focus = false;
                event.accepted = true;

                console.log("Settings | Назначен новый хоткей для " + keybindRoot.settingName + ":", keybindRoot.currentKey)
            }
        }
    }

    // Разделительная полоса строго ПОД виджетом
    Rectangle
    {
        Layout.fillWidth: true
        height: 1
        color: "#3f4147"

        // Четкие отступы: 15px до контента сверху, 15px свободного места снизу
        Layout.topMargin: 15
        Layout.bottomMargin: 15
    }
}

import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import VOX_Client

Dialog
{
    id: dialogRoot
    anchors.centerIn: parent
    width: 440
    height: 340
    modal: true

    // Синхронизируем состояние видимости с C++ UIManager
    visible: UIStateManager.isCreateChannelDialogOpen

    onClosed:
    {
        UIStateManager.isCreateChannelDialogOpen = false
        channelNameInput.text = ""
    }

    background: Rectangle
    {
        color: "#313338"
        radius: 8
        border.color: "#1f2023"
        border.width: 1
    }

    header: Rectangle
    {
        color: "#2b2d31"
        height: 48
        radius: 8

        Text
        {
            text: "Создать канал"
            color: "white"
            font.bold: true
            font.pixelSize: 16
            anchors.centerIn: parent
        }
    }

    contentItem: ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 14

        // -----------------------------------------------------------------
        // ВЫБОР ТИПА КАНАЛА (DISCORD STYLE RADIO BUTTONS)
        // -----------------------------------------------------------------
        Text
        {
            text: "ТИП КАНАЛА"
            color: "#b5bac1"
            font.pixelSize: 11
            font.bold: true
        }

        ButtonGroup { id: typeGroup }

        // Переключатель: Текстовый канал
        Button
        {
            id: textRadioBtn
            Layout.fillWidth: true
            implicitHeight: 46
            checkable: true
            checked: UIStateManager.defaultChannelType === 2
            ButtonGroup.group: typeGroup

            background: Rectangle
            {
                color: textRadioBtn.checked ? "#383a40" : (textRadioBtn.hovered ? "#35373c" : "#2b2d31")
                radius: 6
                border.color: textRadioBtn.checked ? "#5865f2" : "transparent"
                border.width: 1
            }

            contentItem: RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                Text { text: "#"; color: "white"; font.pixelSize: 18 }

                Column
                {
                    Layout.fillWidth: true
                    Text { text: "Текстовый"; color: "white"; font.bold: true; font.pixelSize: 13 }
                    Text { text: "Отправляйте сообщения, изображения, мнения и эмодзи"; color: "#949ba4"; font.pixelSize: 11 }
                }

                RadioButton
                {
                    checked: textRadioBtn.checked
                    ButtonGroup.group: typeGroup
                    activeFocusOnTab: false
                    enabled: false

                    indicator: Rectangle {
                        implicitWidth: 20
                        implicitHeight: 20
                        radius: 10
                        border.color: textRadioBtn.checked ? "#5865f2" : "#b5bac1"
                        border.width: textRadioBtn.checked ? 6 : 2
                        color: "transparent"
                    }
                }
            }
        }

        // Переключатель: Голосовой канал
        Button
        {
            id: voiceRadioBtn
            Layout.fillWidth: true
            implicitHeight: 46
            checkable: true
            checked: UIStateManager.defaultChannelType === 5
            ButtonGroup.group: typeGroup

            background: Rectangle
            {
                color: voiceRadioBtn.checked ? "#383a40" : (voiceRadioBtn.hovered ? "#35373c" : "#2b2d31")
                radius: 6
                border.color: voiceRadioBtn.checked ? "#5865f2" : "transparent"
                border.width: 1
            }

            contentItem: RowLayout
            {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10
                Text { text: "🔊"; color: "white"; font.pixelSize: 16 }

                Column
                {
                    Layout.fillWidth: true
                    Text { text: "Голосовой"; color: "white"; font.bold: true; font.pixelSize: 13 }
                    Text { text: "Общайтесь голосом, используйте демонстрацию экрана"; color: "#949ba4"; font.pixelSize: 11 }
                }

                RadioButton
                {
                    checked: voiceRadioBtn.checked
                    ButtonGroup.group: typeGroup
                    activeFocusOnTab: false
                    enabled: false

                    indicator: Rectangle {
                        implicitWidth: 20
                        implicitHeight: 20
                        radius: 10
                        border.color: voiceRadioBtn.checked ? "#5865f2" : "#b5bac1"
                        border.width: voiceRadioBtn.checked ? 6 : 2
                        color: "transparent"
                    }
                }
            }
        }

        // -----------------------------------------------------------------
        // ВВОД ИМЕНИ КАНАЛА
        // -----------------------------------------------------------------
        Text
        {
            text: "НАЗВАНИЕ КАНАЛА"
            color: "#b5bac1"
            font.pixelSize: 11
            font.bold: true
            Layout.topMargin: 4
        }

        TextField
        {
            id: channelNameInput
            Layout.fillWidth: true
            placeholderText: textRadioBtn.checked ? "новая-комната" : "Новый голосовой канал"
            color: "white"
            placeholderTextColor: "#949ba4"
            maximumLength: 32

            background: Rectangle {
                color: "#1e1f22"
                radius: 4
                border.color: channelNameInput.activeFocus ? "#5865f2" : "transparent"
                border.width: 1
            }
        }

        // -----------------------------------------------------------------
        // УПРАВЛЯЮЩИЕ КНОПКИ
        // -----------------------------------------------------------------
        RowLayout
        {
            Layout.alignment: Qt.AlignRight
            Layout.topMargin: 8
            spacing: 12

            Button
            {
                text: "Отмена"
                onClicked: dialogRoot.close()
                contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 13 }
                background: Rectangle { implicitWidth: 80; implicitHeight: 34; color: "#4e5058"; radius: 4 }
                HoverHandler { cursorShape: Qt.PointingHandCursor }
            }

            Button
            {
                text: "Создать"
                enabled: channelNameInput.text.trim().length > 0

                onClicked: {
                    var finalType = textRadioBtn.checked ? 2 : 5;
                    console.log("UI | Создание канала через отдельный виджет. Тип:", finalType, "Имя:", channelNameInput.text);

                    ChatManager.openOrCreateChatFromUI(
                        finalType,
                        "",
                        ServerManager.activeServerGUID,
                        channelNameInput.text
                    );

                    dialogRoot.close();
                }

                contentItem: Text { text: parent.text; color: "white"; font.bold: true; font.pixelSize: 13 }
                background: Rectangle {
                    implicitWidth: 80;
                    implicitHeight: 34;
                    color: parent.enabled ? "#5865f2" : "#3c4270"
                    radius: 4
                }
                HoverHandler { cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor }
            }
        }
    }
}

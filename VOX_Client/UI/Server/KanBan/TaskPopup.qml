import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog
{
    id: dialog

    property string taskTitle: ""
    property string columnTitle: ""

    anchors.centerIn: parent
    width: 750
    height: 550
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    Overlay.modal: Rectangle
    {
    color: "#aa000000"
    }

    background: Rectangle
    {
        color: "#313338"
        radius: 12
        border.color: "#1f2023"
        border.width: 1
    }

    ListModel
    {
        id: commentsModel
        ListElement { user: "Дмитрий (PM)"; commentText: "Коллеги, нужно ускорить эту задачу к пятнице."; time: "14:20" }
        ListElement { user: "Разработчик"; commentText: "Уже в процессе, разделяю слои в QML."; time: "15:05" }
    }

    // Контент окна
    header: Item
    {
        implicitHeight: 70
        RowLayout
        {
            anchors.fill: parent
            anchors.margins: 20

            ColumnLayout
            {
                spacing: 4
                Text
                {
                    text: "📋 " + dialog.taskTitle
                    color: "white"
                    font.bold: true
                    font.pixelSize: 18
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                }
                Text
                {
                    text: "в колонке " + dialog.columnTitle
                    color: "#949ba4"
                    font.pixelSize: 12
                }
            }

            Item { Layout.fillWidth: true }

            // Кнопка закрытия
            Button
            {
                flat: true
                implicitWidth: 32
                implicitHeight: 32
                background: Rectangle { color: parent.hovered ? "#35373c" : "transparent"; radius: 16 }
                contentItem: Text { text: "✕"; color: "#b5bac1"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: dialog.close()
            }
        }
    }

    contentItem: RowLayout
    {
        spacing: 20
        // ЛЕВАЯ СТОРОНА: Описание + Чат
        ColumnLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16

            // Описание задачи
            ColumnLayout
            {
                Layout.fillWidth: true
                spacing: 6
                Text { text: "Описание"; color: "white"; font.bold: true; font.pixelSize: 13 }
                Rectangle
                {
                    Layout.fillWidth: true
                    height: 60
                    color: "#2b2d31"
                    radius: 6

                    Text
                    {
                        anchors.fill: parent
                        anchors.margins: 10
                        text: "Разработать прототип бизнес-инструмента, объединяющего функционал канбан-досок и каналов связи."
                        color: "#dbdee1"
                        font.pixelSize: 12
                        wrapMode: Text.Wrap
                    }
                }
            }

            // Лента комментариев
            ColumnLayout
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 8

                Text { text: "Комментарии и обсуждение"; color: "white"; font.bold: true; font.pixelSize: 13 }

                Rectangle
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#2b2d31"
                    radius: 6
                    clip: true

                    ListView
                    {
                        id: commentsListView
                        anchors.fill: parent
                        anchors.margins: 10
                        model: commentsModel
                        spacing: 10
                        clip: true

                        delegate: RowLayout
                        {
                            width: commentsListView.width
                            spacing: 10

                            Rectangle
                            {
                                width: 28; height: 28; radius: 14
                                color: user.indexOf("PM") !== -1 ? "#f5a623" : "#5865f2"
                                Text { text: user[0]; color: "white"; font.bold: true; anchors.centerIn: parent }
                            }

                            ColumnLayout
                            {
                                Layout.fillWidth: true
                                spacing: 2
                                RowLayout
                                {
                                    Text { text: user; color: "white"; font.bold: true; font.pixelSize: 12 }
                                    Text { text: time; color: "#949ba4"; font.pixelSize: 10 }
                                }
                                Text { text: commentText; color: "#dbdee1"; font.pixelSize: 12; wrapMode: Text.Wrap; Layout.fillWidth: true }
                            }
                        }

                        onCountChanged: Qt.callLater(commentsListView.positionViewAtEnd)
                    }
                }
            }

            // Поле ввода комментария
            Rectangle
            {
                Layout.fillWidth: true
                height: 38
                color: "#383a40"
                radius: 6

                RowLayout
                {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 6

                    TextField
                    {
                        id: commentInput
                        Layout.fillWidth: true
                        placeholderText: "Написать комментарий..."
                        placeholderTextColor: "#646970"
                        color: "#dbdee1"
                        font.pixelSize: 13
                        background: Item {}

                        onAccepted:
                        {
                            if (text.trim() !== "")
                            {
                                commentsModel.append({
                                    user: "Вы (Разработчик)",
                                    commentText: text,
                                    time: new Date().toLocaleTimeString(Qt.locale("ru_RU"), "hh:mm")
                                })
                                text = ""
                            }
                        }
                    }

                    Button
                    {
                        implicitWidth: 26; implicitHeight: 26
                        flat: true
                        background: Rectangle { color: parent.hovered ? "#4752c4" : "#5865f2"; radius: 4 }
                        contentItem: Text { text: "➔"; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        onClicked: commentInput.accepted()
                    }
                }
            }
        }

        // ПРАВАЯ СТОРОНА: Метаданные
        ColumnLayout
        {
            Layout.preferredWidth: 140
            Layout.fillHeight: true
            spacing: 12
            Layout.alignment: Qt.AlignTop

            Text
            {
                text: "ИСПОЛНИТЕЛЬ";
                color: "#949ba4";
                font.bold: true;
                font.pixelSize: 10
            }

            RowLayout
            {
                Rectangle
                {
                    width: 20;
                    height: 20;
                    radius: 10;
                    color: "#5865f2"
                }
                Text
                {
                    text: "Разработчик";
                    color: "#dbdee1";
                    font.pixelSize: 12
                }
            }

            Item { height: 4 }

            Text
            {
                text: "ПРИОРИТЕТ";
                color: "#949ba4";
                font.bold: true;
                font.pixelSize: 10
            }

            Rectangle
            {
                width: 70;
                height: 20;
                radius: 4;
                color: "#248046"
                Text
                {
                    text: "Высокий";
                    color: "white";
                    font.bold: true;
                    font.pixelSize: 11;
                    anchors.centerIn: parent
                }
            }

            Item
            {
                Layout.fillHeight: true
            }
        }
    }
}

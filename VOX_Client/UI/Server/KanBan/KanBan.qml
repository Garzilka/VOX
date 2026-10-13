import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item
{
    id: root

    // Основная модель Kanban-доски
    ListModel
    {
        id: boardModel
        ListElement
        {
            columnTitle: "БЭКЛОГ"
            tasks:
            [
                ListElement
                {
                    taskText: "Интеграция с C++ API"
                },
                ListElement
                {
                    taskText: "Настроить права доступа"
                }
            ]
        }
        ListElement
        {
            columnTitle: "В РАБОТЕ"
            tasks:
            [
                ListElement
                {
                    taskText: "Разработать интерфейс Kanban"
                }
            ]
        }
        ListElement
        {
            columnTitle: "ГОТОВО"
            tasks:
            [
                ListElement
                {
                    taskText: "Спроектировать ToolBar"
                }
            ]
        }
    }

    property string selectedTaskText: ""
    property string selectedColumnTitle: ""

    // Горизонтальный список для КОЛОНОК
    ListView
    {
        id: columnsListView
        anchors.fill: parent
        anchors.margins: 20
        orientation: ListView.Horizontal
        spacing: 16
        model: boardModel
        clip: true

        delegate: Rectangle
        {
            id: columnContainer
            width: 280
            height: columnsListView.height - 20
            color: "#2b2d31"
            radius: 8
            property string currentColumnName: columnTitle

            ColumnLayout
            {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                RowLayout
                {
                    Layout.fillWidth: true
                    Text
                    {
                        text: columnTitle;
                        color: "white";
                        font.bold: true;
                        font.pixelSize: 14
                    }
                    Item
                    {
                        Layout.fillWidth: true
                    }
                    Text
                    {
                        text: tasksListView.count;
                        color: "#949ba4";
                        font.pixelSize: 12
                    }
                }

                ListView
                {
                    id: tasksListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 8
                    model: tasks
                    clip: true

                    delegate: Rectangle
                    {
                        width: tasksListView.width
                        height: 60
                        color: "#313338"
                        radius: 6
                        border.color: cardMouseArea.containsMouse ? "#5865f2" : "transparent"
                        border.width: 1

                        MouseArea
                        {
                            id: cardMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked:
                            {
                                root.selectedTaskText = taskText
                                root.selectedColumnTitle = columnContainer.currentColumnName
                                taskModal.open()
                            }
                        }

                        ColumnLayout
                        {
                            anchors.fill: parent
                            anchors.margins: 10
                            Text
                            {
                                text: taskText
                                color: "#dbdee1"
                                font.pixelSize: 13
                                wrapMode: Text.Wrap
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }

    // ==========================================
    // МОДАЛЬНОЕ ОКНО (Popup) С ЧАТОМ
    // ==========================================
    TaskPopup
    {
        id: taskModal
    }
}

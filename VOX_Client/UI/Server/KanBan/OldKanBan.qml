color: "#313338"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16

            // Колонки задач
            Rectangle {
                Layout.fillHeight: true; Layout.preferredWidth: 250; color: "#2b2d31"; radius: 8
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 12
                    Text { text: "В РАБОТЕ (2)"; color: "white"; font.bold: true }
                    Rectangle { Layout.fillWidth: true; height: 60; color: "#383a40"; radius: 6; Text { text: "Сверстать новый ToolBar"; color: "white"; anchors.centerIn: parent } }
                    Rectangle { Layout.fillWidth: true; height: 60; color: "#383a40"; radius: 6; Text { text: "Миграция на Qt 6"; color: "white"; anchors.centerIn: parent } }
                    Item { Layout.fillHeight: true }
                }
            }
            Rectangle {
                Layout.fillHeight: true;
                Layout.preferredWidth: 250;
                color: "#2b2d31";
                radius: 8

                ColumnLayout {
                    anchors.fill: parent;
                    anchors.margins: 12
                    Text {
                        text: "ГОТОВО (1)";
                        color: "white";
                        font.bold: true
                    }
                    Rectangle {
                        Layout.fillWidth: true;
                        height: 60;
                        color: "#383a40";
                        radius: 6;

                        Text {
                            text: "Создать прототип";
                            color: "white";
                            anchors.centerIn: parent
                        }
                    }
                    Item { Layout.fillHeight: true }}}
            Item {
                Layout.fillWidth: true
            }
        }
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout
{
    id: resultListRoot
    spacing: 0

    Rectangle
    {
        anchors.fill: parent
        color: "transparent"
        border.color: "#2b2d31"
        border.width: 1
        radius: 8
        visible: friendsListView.count === 0

        ColumnLayout
        {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "🔍 Здесь будут отображаться результаты поиска"
                color: "#949ba4"
                font.pixelSize: 15
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "Введи никнейм выше и нажми Enter или кнопку Поиск"
                color: "#4e5058"
                font.pixelSize: 13
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    ListView
    {
        id: friendsListView
        anchors.fill: parent
        Layout.fillWidth: true
        Layout.fillHeight: true
        model: FriendsManager.searchResults // Теперь это массив UUID строк
        spacing: 8
        clip: true
        visible: count > 0

        delegate: SearchResultItem
        {
            userGuid: modelData ? modelData : ""
        }
    }
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item
{
    id: requestsListWidgetRoot

    // Заглушка, если список пуст
    Rectangle
    {
        anchors.fill: parent
        color: "transparent"
        border.color: "#2b2d31"
        border.width: 1
        radius: 8
        visible: requestsListView.count === 0

        ColumnLayout
        {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "⌛ У вас нет ожидающих заявок"
                color: "#949ba4"
                font.pixelSize: 15
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    // Лента заявок
    ListView
    {
        id: requestsListView
        anchors.fill: parent
        model: FriendsManager.pendingRequests
        spacing: 8
        clip: true
        visible: count > 0

        delegate: RequestsItemWidget
        {
            width: requestsListView.width
            userGuid: (modelData && modelData["guid"]) ? modelData["guid"] : ""
            isIncoming: (modelData && modelData["direction"] === "incoming")
        }
    }
}

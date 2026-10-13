import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item
{
    anchors.fill: parent
    ListView
    {
        anchors.fill: parent
        Layout.topMargin: 8
        clip: true

        model: ["📊 Спринт текущий", "🎯 Бэклог продукта", "🐛 Баг-трекер"]

        delegate: ItemDelegate
        {
            height: 34
            contentItem: Text
            {
                text: modelData
                color: hovered ? "#dbdee1" : "#949ba4"
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 13
            }
            background: Rectangle
            {
                color: hovered ? "#35373c" : "transparent"
                radius: 4
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
            }
        }
    }
}

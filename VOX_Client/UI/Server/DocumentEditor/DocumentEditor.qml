import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Item
{
    ColumnLayout
    {
        anchors.fill: parent;
        anchors.margins: 40

        Text
        {
            text: "📝 База знаний: Инструкция по проекту";
            color: "white";
            font.bold: true;
            font.pixelSize: 24
        }

        TextArea
        {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            color: "#dbdee1"
            text: "Добро пожаловать в корпоративную Wiki.\n\nЗдесь можно описывать спринты и вести документацию."
            background: Rectangle
            {
                color: "#2b2d31";
                radius: 8
            }
        }
    }
}

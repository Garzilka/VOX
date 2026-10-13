import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item
{
    id: inputSettingsRoot
    anchors.fill: parent

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 20

        // Отступы для контента внутри центрального блока
        Layout.leftMargin: 40
        Layout.rightMargin: 20
        Layout.topMargin: 60
        Layout.bottomMargin: 40

        // Заголовок страницы
        Text
        {
            text: "Настройки голоса"
            color: "white"
            font.pixelSize: 20
            font.bold: true
            font.family: "GG Sans, Noto Sans, Helvetica Neue, sans-serif"

            Layout.fillWidth: true
            Layout.bottomMargin: 10
            horizontalAlignment: Text.AlignHCenter
        }

        // ==========================================
        // БИНДИНГИ
        // ==========================================
        KeybindInput
        {
            settingName: "Переключить микрофон"
        }

        KeybindInput
        {
            settingName: "Активация микрофона"
        }

        // Выталкивает все настройки наверх
        Item { Layout.fillHeight: true }
    }
}

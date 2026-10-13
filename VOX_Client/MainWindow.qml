import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import VOX_Client

ApplicationWindow
{
    id: window
    width: 950
    height: 600
    minimumWidth: 500
    minimumHeight: 400
    visible: false
    title: qsTr("VOX")
    flags: Qt.Window | Qt.FramelessWindowHint

    onVisibleChanged:
    {
        if (visible) {
            openAnimation.start();
        }
    }
    Component.onCompleted:
    {
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2
    }

    ParallelAnimation
    {
        id: openAnimation
        NumberAnimation { target: windowContent; property: "scale"; from: 0.92; to: 1.0; duration: 350; easing.type: Easing.OutBack }
        NumberAnimation { target: windowContent; property: "opacity"; from: 0.0; to: 1.0; duration: 250; easing.type: Easing.OutCubic }
    }

    SequentialAnimation
    {
        id: stateChangeAnimation

        ParallelAnimation
        {
            NumberAnimation { target: mainStackLayout; property: "opacity"; to: 0.0; duration: 150; easing.type: Easing.OutCubic }
            NumberAnimation { target: mainStackLayout; property: "yOffset"; to: 15; duration: 150; easing.type: Easing.OutCubic }
        }

        PropertyAction
        {
            target: mainStackLayout
            property: "currentIndex"
            value: mainStackLayout.pendingIndex
        }

        ParallelAnimation
        {
            NumberAnimation { target: mainStackLayout; property: "opacity"; to: 1.0; duration: 250; easing.type: Easing.OutQuad }
            NumberAnimation { target: mainStackLayout; property: "yOffset"; to: 0; duration: 250; easing.type: Easing.OutCubic }
        }
    }

    Item
    {
        id: windowContent
        anchors.fill: parent
        transformOrigin: Item.Center
        opacity: 0.0
        scale: 0.92

        TitleBar
        {
            id: titleBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            mainAppWindow: window
        }

        Item
        {
            id: mainStackLayout
            anchors.top: titleBar.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            property int yOffset: 0
            property int currentIndex: 0
            property int pendingIndex: 0

            Component.onCompleted: {
                currentIndex = UIStateManager.MainUIState; // Синхронизируем глобальный индекс перехода
                pendingIndex = UIStateManager.MainUIState;
            }

            Connections
            {
                target: UIStateManager
                // Слушаем изменение главных вкладок приложения (Auth / Workspace / Settings)
                function onMainUIStateChanged()
                {
                    if (mainStackLayout.currentIndex !== UIStateManager.MainUIState) {
                        mainStackLayout.pendingIndex = UIStateManager.MainUIState;
                        stateChangeAnimation.start();
                    }
                }
            }

            function navigateTo(index) {
                if (mainStackLayout.currentIndex !== index) {
                    mainStackLayout.pendingIndex = index;
                    stateChangeAnimation.start();
                }
            }

            // ==========================================================
            // Экраны приложения (Завязаны на MainUIState)
            // ==========================================================
            LoginScreen
            {
                anchors.fill: parent
                y: mainStackLayout.currentIndex === 0 ? mainStackLayout.yOffset : 0
                visible: mainStackLayout.currentIndex === 0 || mainStackLayout.pendingIndex === 0
            }

            MainWorkspace
            {
                anchors.fill: parent
                y: mainStackLayout.currentIndex === 1 ? mainStackLayout.yOffset : 0
                visible: mainStackLayout.currentIndex === 1 || mainStackLayout.pendingIndex === 1
            }

            Settings
            {
                anchors.fill: parent
                y: mainStackLayout.currentIndex === 2 ? mainStackLayout.yOffset : 0
                visible: mainStackLayout.currentIndex === 2 || mainStackLayout.pendingIndex === 2

                onVisibleChanged: {
                    if (visible) {
                        forceActiveFocus()
                    }
                }
            }
        }
    }
}

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import VOX_Client

Item
{
    id: workspaceRoot
    anchors.fill: parent

    property int menuPanelWidth: 240
    property int rightPanelWidth: 240

    RowLayout
    {
        anchors.fill: parent
        spacing: 0

        // Блок 1: Основная навигация
        Rectangle {
            Layout.fillHeight: true; Layout.preferredWidth: 72; color: "#1e1f22"
            MainLeftPannel { anchors.fill: parent }
        }

        // Блок 2: Динамическое меню
        Rectangle {
            Layout.fillHeight: true; Layout.preferredWidth: workspaceRoot.menuPanelWidth; color: "#2b2d31"
            ColumnLayout {
                anchors.fill: parent; spacing: 0
                Item {
                    Layout.fillWidth: true; Layout.fillHeight: true
                    Loader {
                        id: menuLoader; anchors.fill: parent
                        sourceComponent: UIStateManager.WorkState === 0 ? friendsMenuComp : chatsMenuComp
                    }
                }
                Rectangle { Layout.fillWidth: true; height: 1; color: "#1f2023" }
                UserBar {}
            }
            Component { id: friendsMenuComp; FriendsPannel {} }
            Component { id: chatsMenuComp; ServerPannel {} }
        }

        Rectangle { color: "#1f2023"; Layout.fillHeight: true; implicitWidth: 1 }

        // Блок 3: Рабочая зона
        Rectangle {
            Layout.fillHeight: true; Layout.fillWidth: true; color: "#313338"
            Loader {
                id: workspaceLoader; anchors.fill: parent
                sourceComponent: UIStateManager.WorkState === 0 ? friendsWorkSpace : serverWorkspaceComp
            }
            Component { id: friendsWorkSpace; FriendsWorkSpace {} }
            Component { id: serverWorkspaceComp; ServerWorkSpace {} }
        }

        // Блок 4: Список участников
        Rectangle {
            id: rightParticipantsPanel
            Layout.fillHeight: true; Layout.preferredWidth: 240; color: "#2b2d31"
            visible: UIStateManager.WorkState === 1 && ServerManager.State === 0
            Members { anchors.fill: parent }
        }
    }

    // =========================================================================
    // ГЛОБАЛЬНЫЕ ДИАЛОГИ (ЦЕНТРИРУЮТСЯ ПО ВСЕМУ ЭКРАНУ)
    // =========================================================================
    CreateChannelDialog
    {
        id: globalCreateChannelDialog
    }

    CreateServerDialog
    {
        id: globalCreateServerDialog
    }
}

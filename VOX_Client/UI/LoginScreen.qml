import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Item
{
    Component.onCompleted:
    {
        ClientConnection.checkSavedSession();
    }
    readonly property color colorBg: "#1e1f22"
    readonly property color colorAuth: "#313338"
    readonly property color colorText: "#b5bac1"
    readonly property color colorError: "#fa777c" // Красный цвет для ошибок в стиле Discord

    id: loginScreen
    anchors.fill: parent

    property bool isLoginMode: true
    property string errorMessage: "" // Хранит текст текущей ошибки

    // Слушаем сигналы из C++ объекта ClientConnection
    Connections
    {
        target: ClientConnection
        function onAuthErrorReceived()
        {
            if (loginScreen.isLoginMode)
            {
                loginScreen.errorMessage = "Неверный логин или пароль";
            } else
            {
                loginScreen.errorMessage = "Данный пользователь уже существует";
            }
        }
    }

    // Вынесенная функция отправки формы
    function submitForm()
    {
        loginScreen.errorMessage = ""; //
        if (loginScreen.isLoginMode)
        {
            // Передаем состояние rememberMe.text / checked
            ClientConnection.requestAuthorization(usernameField.text, passwordField.text, rememberMeCheckBox.checked);
            console.log("Попытка входа:", usernameField.text); //
        }
        else
        {
            ClientConnection.requestRegistration(usernameField.text, emailField.text, passwordField.text, rememberMeCheckBox.checked);
            console.log("Регистрация пользователя:", usernameField.text, "Email:", emailField.text); //
        }
    }

    Rectangle
    {
        anchors.fill: parent
        color: colorBg

        Rectangle
        {
            id: authCard
            anchors.centerIn: parent
            width: 480
            height: isLoginMode ? (errorMessage ? 460 : 430) : (errorMessage ? 540 : 510)
            color: colorAuth
            radius: 8

            Behavior on height { NumberAnimation { duration: 150 } }

            ColumnLayout
            {
                anchors.fill: parent
                anchors.margins: 32
                spacing: 16 // Чуть уменьшил spacing, чтобы вместить ошибку

                // ==========================================
                // Заголовки
                // ==========================================
                ColumnLayout
                {
                    Layout.fillWidth: true
                    spacing: 8

                    Text
                    {
                        text: isLoginMode ? "С возвращением!" : "Создать учетную запись"
                        font.pixelSize: 24
                        font.bold: true
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                    }

                    Text
                    {
                        text: isLoginMode ? "Мы рады снова вас видеть!" : "Пожалуйста, заполните форму ниже"
                        font.pixelSize: 14
                        color: colorText
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                    }
                }

                // ==========================================
                // Поля ввода
                // ==========================================
                ColumnLayout
                {
                    spacing: 12
                    Layout.fillWidth: true

                    // Поле EMAIL
                    ColumnLayout
                    {
                        spacing: 6
                        Layout.fillWidth: true
                        visible: !loginScreen.isLoginMode

                        Text
                        {
                            text: "АДРЕС ЭЛЕКТРОННОЙ ПОЧТЫ"
                            font.pixelSize: 12
                            font.bold: true
                            color: colorText
                        }

                        TextField
                        {
                            id: emailField
                            Layout.fillWidth: true
                            implicitHeight: 40
                            color: "#f2f3f5"
                            placeholderTextColor: "#949ba4"
                            placeholderText: "example@email.com"
                            background: Rectangle
                            {
                                color: "#1e1f22"
                                radius: 4
                                border.color: emailField.activeFocus ? "#5865f2" : "transparent"
                                border.width: 1
                            }
                            onAccepted: usernameField.forceActiveFocus()
                            onTextChanged: loginScreen.errorMessage = ""
                        }
                    }

                    // ==========================================
                    // Поле ИМЯ ПОЛЬЗОВАТЕЛЯ
                    // ==========================================
                    ColumnLayout
                    {
                        spacing: 6
                        Layout.fillWidth: true

                        Text
                        {
                            text: loginScreen.isLoginMode ? "ДАННЫЕ ВХОДА (ИМЯ)" : "ИМЯ ПОЛЬЗОВАТЕЛЯ"
                            font.pixelSize: 12
                            font.bold: true
                            color: colorText
                        }

                        TextField
                        {
                            id: usernameField
                            Layout.fillWidth: true
                            implicitHeight: 40
                            color: "#f2f3f5"
                            placeholderTextColor: "#949ba4"

                            // ЭТОТ ВАЛИДАТОР запретит вводить пробелы, русские буквы и спецсимволы прямо при наборе!
                            validator: RegularExpressionValidator { regularExpression: /^[a-zA-Z0-9_]{0,20}$/ }

                            background: Rectangle {
                                color: "#1e1f22"
                                radius: 4
                                border.color: usernameField.activeFocus ? "#5865f2" : "transparent"
                                border.width: 1
                            }
                            onAccepted: passwordField.forceActiveFocus()
                            onTextChanged: loginScreen.errorMessage = ""
                        }
                    }

                    // ==========================================
                    // Поле ПАРОЛЬ
                    // ==========================================
                    ColumnLayout
                    {
                        spacing: 6
                        Layout.fillWidth: true

                        Text
                        {
                            text: "ПАРОЛЬ"
                            font.pixelSize: 12
                            font.bold: true
                            color: colorText
                        }

                        TextField
                        {
                            id: passwordField
                            Layout.fillWidth: true
                            implicitHeight: 40
                            echoMode: TextInput.Password
                            color: "#f2f3f5"
                            placeholderTextColor: "#949ba4"
                            background: Rectangle
                            {
                                color: "#1e1f22"
                                radius: 4
                                border.color: passwordField.activeFocus ? "#5865f2" : "transparent"
                                border.width: 1
                            }
                            onAccepted: if(submitButton.enabled) loginScreen.submitForm()
                            onTextChanged: loginScreen.errorMessage = ""
                        }
                    }
                }
                // ==========================================
                // ЧЕКБОКС "ЗАПОМНИТЬ МЕНЯ"
                // ==========================================
                CheckBox
                {
                    id: rememberMeCheckBox
                    Layout.fillWidth: true
                    text: "Запомнить меня"
                    checked: true // По умолчанию включен в стиле лаунчеров

                    // Стилизация под темную тему твоего чата
                    indicator: Rectangle
                    {
                        implicitWidth: 18
                        implicitHeight: 18
                        x: rememberMeCheckBox.leftPadding
                        y: parent.height / 2 - height / 2
                        radius: 3
                        color: "#1e1f22"
                        border.color: rememberMeCheckBox.checked ? "#5865f2" : "#949ba4"
                        border.width: 1

                        Rectangle
                        {
                            width: 10
                            height: 10
                            x: 4
                            y: 4
                            radius: 1
                            color: "#5865f2"
                            visible: rememberMeCheckBox.checked
                        }
                    }

                    contentItem: Text {
                        text: rememberMeCheckBox.text
                        font.pixelSize: 13
                        color: rememberMeCheckBox.hovered ? "#ffffff" : colorText
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: rememberMeCheckBox.indicator.width + 8
                    }

                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                }
                // ==========================================
                // БЛОК ОШИБКИ
                // ==========================================
                Text
                {
                    id: errorLabel
                    text: loginScreen.errorMessage
                    color: loginScreen.colorError
                    font.pixelSize: 13
                    font.bold: true
                    visible: loginScreen.errorMessage !== ""
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap // Ошибка может быть длинной, пусть переносится
                    horizontalAlignment: Text.AlignLeft
                }

                // ==========================================
                // Кнопки управления
                // ==========================================
                ColumnLayout
                {
                    spacing: 12
                    Layout.fillWidth: true

                    Button
                    {
                        id: submitButton
                        Layout.fillWidth: true
                        implicitHeight: 44

                        // Вычисляем, валидна ли форма прямо сейчас для нажатия кнопки
                        readonly property bool isFormValid: {
                            if (loginScreen.isLoginMode) {
                                return usernameField.text.length >= 3 && passwordField.text.length >= 4;
                            } else {
                                // Для регистрации проверяем еще и Email простейшей регуляркой на клиенте
                                var emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
                                return usernameField.text.length >= 3 &&
                                       passwordField.text.length >= 4 &&
                                       emailPattern.test(emailField.text);
                            }
                        }

                        // Кнопка тускнеет и блокируется, если условия валидации не выполнены
                        enabled: isFormValid
                        opacity: enabled ? 1.0 : 0.5

                        HoverHandler
                        {
                            cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                        }

                        background: Rectangle
                        {
                            color: parent.enabled ? (parent.hovered ? "#4752c4" : "#5865f2") : "#3c4270"
                            radius: 3
                        }

                        contentItem: Text
                        {
                            text: loginScreen.isLoginMode ? "Вход" : "Продолжить"
                            color: parent.enabled ? "#ffffff" : "#949ba4"
                            font.pixelSize: 16
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: loginScreen.submitForm()
                    }
                    RowLayout
                    {
                        Layout.alignment: Qt.AlignLeft
                        spacing: 4
                        Text
                        {
                            text: loginScreen.isLoginMode ? "Нужна учетная запись?" : "Уже есть учетная запись?"
                            color: "#949ba4"
                            font.pixelSize: 13
                        }

                        Text
                        {
                            text: loginScreen.isLoginMode ? "Зарегистрироваться" : "Войти"
                            color: "#00a8fc"
                            font.pixelSize: 13
                            MouseArea
                            {
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked:
                                {
                                    loginScreen.isLoginMode = !loginScreen.isLoginMode
                                    loginScreen.errorMessage = "" // Сбрасываем ошибку при переключении окон
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

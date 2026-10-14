cmake_minimum_required(VERSION 3.16)

project(VOX_Client VERSION 0.0.1.6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим Qt6 и его медиа-компоненты
find_package(Qt6 REQUIRED COMPONENTS Quick Core Network Multimedia Concurrent)
find_package(OpenSSL REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

# Подключаем дизайн Figma (если есть)
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

# Группируем C++ исходники для корректной работы AUTOMOC под MSVC
set(CLIENT_SOURCES

    resources.rc
    main.cpp
    Core/connection.cpp
    Core/connection.h
    Core/Events/newmessage.cpp
    Core/Events/newmessage.h
    Core/Data/CoreData.h
    Core/Lib/cryptolib.h
    Core/Lib/cryptolib.cpp
    Core/UIManager/uimanager.h
    Core/UIManager/uimanager.cpp
    Core/UIManager/Server/servermanager.h
    Core/UIManager/Server/servermanager.cpp
    Core/UIManager/Friend/friendmanager.h
    Core/UIManager/Friend/friendmanager.cpp
    Core/UIManager/ChatManager/chatmanager.h
    Core/UIManager/ChatManager/chatmanager.cpp
    Core/UIManager/NotificationManager/notificationmanager.h
    Core/UIManager/NotificationManager/notificationmanager.cpp
    Core/UIManager/UsersManager/usersmanager.h
    Core/UIManager/UsersManager/usersmanager.cpp
    Core/Data/FriendData.h
    Core/Data/DataIncluds.h
    Core/Data/ServerData.h
    Core/Data/ChannelsData.h
    Core/ServerLogic.cpp
    Core/FriendLogic.cpp
    Core/ChannelsLogic.cpp
    Core/VoiceEngine/voiceengine.h
    Core/VoiceEngine/voiceengine.cpp
    Core/VoiceEngine/miccapture.cpp
    Core/VoiceEngine/miccapture.h
    Core/VoiceEngine/voiceroomdelegate.h
    Core/VoiceEngine/voiceroomdelegate.cpp
    Core/VoiceEngine/playbackworker.h
    Core/VoiceEngine/playbackworker.cpp
)

# Объявляем исполняемый файл с полным списком исходников
qt_add_executable(VOX_Client
    ${CLIENT_SOURCES}
)

# Чистая структура QML модуля
qt_add_qml_module(VOX_Client
    URI VOX_Client
    VERSION 1.0

    QML_FILES
        Main.qml
        MainWindow.qml
        UI/LoginScreen.qml
        UI/MainWorkspace.qml
        UI/MainLeftPannel.qml
        UI/Settings/Settings.qml

        #============= SERVER ==========#

        UI/Server/ServerPannel.qml
        UI/Server/ServerWorkSpace.qml

        UI/Server/KanBan/Boards.qml
        UI/Server/KanBan/KanBan.qml
        UI/Server/KanBan/TaskPopup.qml

        UI/Server/DocumentEditor/Documents.qml
        UI/Server/DocumentEditor/DocumentEditor.qml

        UI/Server/ServerChat/Chats.qml
        UI/Server/ServerChat/TextChannelButton.qml
        UI/Server/ServerChat/VoiceChannelButton.qml
        UI/Server/ServerChat/ChannelCategoryHeader.qml

        #============= FRIENDS ==========#

        UI/Friends/Friends.qml
        UI/Friends/FriendsPannel.qml
        UI/Friends/FriendsWorkSpace.qml

        UI/Friends/FriendList/FriendList.qml
        UI/Friends/FriendList/FriendListItem.qml
        UI/Friends/Search/FriendSearchWidget.qml

        UI/Friends/Requestlist/FriendRequestsWidget.qml

        UI/Friends/DirectChatsList/DirectChatsList.qml
        UI/Friends/DirectChatsList/DirectChatRow.qml

        #============= COMMON ==========#
        UI/Common/TitleBar/TitleBar.qml
        UI/Common/TitleBar/UserBar.qml
        UI/Common/Members/Members.qml
        UI/Common/Chat/ChatSpace.qml
        UI/Common/Chat/MessageList.qml
        UI/Common/Chat/ChatMessageContainer.qml
        UI/Common/Chat/Containers/TextMessageContainer.qml
        UI/Common/Chat/Containers/InvitationMessageContainer.qml

    SOURCES
        ${CLIENT_SOURCES}
        RESOURCES
        QML_FILES UI/Common/Dialogs/CreateChannelDialog.qml
        QML_FILES UI/Common/Dialogs/CreateServerDialog.qml
        QML_FILES UI/Settings/Inputs/InputSettings.qml
        QML_FILES UI/Settings/Inputs/KeybindInput.qml
        SOURCES Core/Data/UserData.h
        SOURCES Core/Data/DifferentsData.h
        QML_FILES UI/Friends/Search/SearchResultItem.qml
        QML_FILES UI/Friends/Search/SearchResultList.qml
        QML_FILES UI/Friends/Requestlist/RequestsListWidget.qml
        QML_FILES UI/Friends/Requestlist/RequestsItemWidget.qml
)

set_target_properties(VOX_Client PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# =====================================================================
# АВТОМАТИЧЕСКИЙ ДЕПЛОЙ LIVEKIT SDK ИЗ ГИТХАБА
# =====================================================================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if(LIVEKIT_LOCAL_SDK_DIR)
    list(PREPEND CMAKE_PREFIX_PATH "${LIVEKIT_LOCAL_SDK_DIR}")
else()
    include(LiveKitSDK)
    livekit_sdk_setup(
    VERSION "1.12.0"  # Фиксируем стабильную совместимую версию
    SDK_DIR "${CMAKE_BINARY_DIR}/_deps/livekit-sdk"
)
endif()

# Находим пакет LiveKit
find_package(LiveKit CONFIG REQUIRED)

# Линкуем модули Qt, OpenSSL и LiveKit
target_link_libraries(VOX_Client PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Network
    Qt6::Multimedia
    Qt6::Concurrent
    OpenSSL::SSL
    OpenSSL::Crypto
    LiveKit::livekit

    # Системные библиотеки Windows для WebRTC звука
    winmm
    shlwapi
    strmiids
)

# Скрипт постобработки сборки для Windows (копирование OpenSSL DLL)
if(WIN32)
    get_target_property(OPENSSL_CRYPTO_DLL OpenSSL::Crypto IMPORTED_LOCATION_RELEASE)
    get_target_property(OPENSSL_SSL_DLL OpenSSL::SSL IMPORTED_LOCATION_RELEASE)

    if(NOT OPENSSL_CRYPTO_DLL)
        get_target_property(OPENSSL_CRYPTO_DLL OpenSSL::Crypto IMPORTED_LOCATION_DEBUG)
    endif()
    if(NOT OPENSSL_SSL_DLL)
        get_target_property(OPENSSL_SSL_DLL OpenSSL::SSL IMPORTED_LOCATION_DEBUG)
    endif()

    if(OPENSSL_CRYPTO_DLL AND OPENSSL_SSL_DLL)
        # Заменено на валидный синтаксис логирования CMake
        message(STATUS "CMake | Настройка автокопирования OpenSSL DLL для VOX_Client успешна.")

        add_custom_command(TARGET VOX_Client POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${OPENSSL_CRYPTO_DLL}"
                "${OPENSSL_SSL_DLL}"
                "$<TARGET_FILE_DIR:VOX_Client>"
            COMMENT "Автоматическое копирование библиотек OpenSSL DLL в папку сборки..."
        )
    else()
        message(WARNING "CMake | Не удалось определить пути к бинарникам OpenSSL DLL для автокопирования!")
    endif()
endif()


include(GNUInstallDirs)
install(TARGETS VOX_Client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

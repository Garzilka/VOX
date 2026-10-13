cmake_minimum_required(VERSION 3.16)

project(VOX_Launcher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)


set(PROJECT_SOURCES
    main.cpp
    launchercontroller.cpp
    launchercontroller.h
    resources.rc # Добавляем RC-файл ресурсов вместо чистого .ico
)

qt_add_executable(VOX_Launcher
    ${PROJECT_SOURCES}
)

qt_add_qml_module(VOX_Launcher
    URI NomadUpdater
    QML_FILES
        Main.qml
)

# Настройки свойств исполняемого файла
set_target_properties(VOX_Launcher PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE # Скрывает консольное окно на Windows
)

target_link_libraries(VOX_Launcher
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS VOX_Launcher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

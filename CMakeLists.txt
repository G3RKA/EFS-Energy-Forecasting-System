cmake_minimum_required(VERSION 3.16)

project(EFS_Project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(MAINMENU MainMenu)
set(ROLE RoleMenu)
set(CUSTOM CustomWidgets)
set(DIALOG DialogWindows)
set(CONFIG Config)


set(PROJECT_HEADERS
    ${CONFIG}/AccountUserRow.h
    ${CONFIG}/AppConfig.h
    ${CONFIG}/AppDataBase.h
    ${CONFIG}/DataBaseManager.h
    ${CONFIG}/LoginLockout.h
    ${MAINMENU}/MainWindow.h
    ${ROLE}/AdminMenu.h
    ${ROLE}/AnalystMenu.h
    ${ROLE}/EnergyMenu.h
    ${CUSTOM}/SystemLabel.h
    ${DIALOG}/LostConnectionDialog.h
    ${DIALOG}/DataMenu.h
    ${DIALOG}/ConsumersMenu.h
    ${DIALOG}/UploadError.h
    ${DIALOG}/AuditMenu.h
    ${DIALOG}/AuditSettings.h
    ${DIALOG}/DataBaseSettings.h
    ${DIALOG}/DataBaseUserSettings.h
    ${DIALOG}/AccountSettings.h
    ${DIALOG}/AccountCreate.h
    ${DIALOG}/AccountDelete.h
    ${DIALOG}/AccountEdit.h
)

set(PROJECT_FORM
    ${MAINMENU}/UI/MainWindow.ui
    ${ROLE}/UI/AdminMenu.ui
    ${ROLE}/UI/AnalystMenu.ui
    ${ROLE}/UI/EnergyMenu.ui
    ${CUSTOM}/UI/SystemLabel.ui
    ${DIALOG}/UI/LostConnectionDialog.ui
    ${DIALOG}/UI/DataMenu.ui
    ${DIALOG}/UI/ConsumersMenu.ui
    ${DIALOG}/UI/UploadError.ui
    ${DIALOG}/UI/AuditMenu.ui
    ${DIALOG}/UI/AuditSettings.ui
    ${DIALOG}/UI/DataBaseSettings.ui
    ${DIALOG}/UI/DataBaseUserSettings.ui
    ${DIALOG}/UI/AccountSettings.ui
    ${DIALOG}/UI/AccountCreate.ui
    ${DIALOG}/UI/AccountEdit.ui
    ${DIALOG}/UI/AccountDelete.ui
)

set(PROJECT_SOURCES
    Main.cpp
    ${CONFIG}/AppConfig.cpp
    ${CONFIG}/AppDataBase.cpp
    ${CONFIG}/DataBaseManager.cpp
    ${CONFIG}/LoginLockout.cpp
    ${MAINMENU}/MainWindow.cpp
    ${ROLE}/AdminMenu.cpp
    ${ROLE}/AnalystMenu.cpp
    ${ROLE}/EnergyMenu.cpp
    ${CUSTOM}/SystemLabel.cpp
    ${DIALOG}/LostConnectionDialog.cpp
    ${DIALOG}/DataMenu.cpp
    ${DIALOG}/ConsumersMenu.cpp
    ${DIALOG}/UploadError.cpp
    ${DIALOG}/AuditMenu.cpp
    ${DIALOG}/AuditSettings.cpp
    ${DIALOG}/DataBaseSettings.cpp
    ${DIALOG}/DataBaseUserSettings.cpp
    ${DIALOG}/AccountSettings.cpp
    ${DIALOG}/AccountCreate.cpp
    ${DIALOG}/AccountDelete.cpp
    ${DIALOG}/AccountEdit.cpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EFS_Project
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_FORM}
        Resources.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET EFS_Project APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(EFS_Project SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(EFS_Project
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(EFS_Project PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.EFS_Project)
endif()
set_target_properties(EFS_Project PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS EFS_Project
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EFS_Project)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${CUSTOM}
    ${CMAKE_CURRENT_SOURCE_DIR}/${ROLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/${DIALOG}
    ${CMAKE_CURRENT_SOURCE_DIR}/bcrypt
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG}
)

find_library(BCRYPT_LIB bcrypt PATHS ${CMAKE_CURRENT_SOURCE_DIR} NO_DEFAULT_PATH)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BCRYPT_LIB})

find_package(Qt6 REQUIRED COMPONENTS Sql)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Sql)


cmake_minimum_required(VERSION 3.20)

project(winver
    VERSION 1.0.0
    DESCRIPTION "Antergos NeXT system information dialog"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)
find_package(KF6 REQUIRED COMPONENTS CoreAddons I18n)

add_executable(winver
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
)

target_link_libraries(winver PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Svg
    KF6::CoreAddons
    KF6::I18n
)

target_compile_definitions(winver PRIVATE
    DATA_DIR="${CMAKE_SOURCE_DIR}/data"
    WINVER_VERSION="${PROJECT_VERSION}"
)

target_include_directories(winver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

install(TARGETS winver RUNTIME DESTINATION bin)
install(FILES data/antergos-logo.png DESTINATION share/winver)

cmake_minimum_required(VERSION 3.16)
project(HotelBookingGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Works with Qt 6 or Qt 5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# The OOP classes are shared with the console version (one folder up)
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(CORE_SOURCES
    ${CORE_DIR}/Person.cpp
    ${CORE_DIR}/Room.cpp
    ${CORE_DIR}/Hotel.cpp
    ${CORE_DIR}/Booking.cpp
    ${CORE_DIR}/InputValidator.cpp
    ${CORE_DIR}/HotelManager.cpp
)

add_executable(HotelBookingGUI
    main.cpp
    MainWindow.h
    MainWindow.cpp
    ${CORE_SOURCES}
)

target_include_directories(HotelBookingGUI PRIVATE ${CORE_DIR})
target_link_libraries(HotelBookingGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# No black console window behind the app on Windows
set_target_properties(HotelBookingGUI PROPERTIES WIN32_EXECUTABLE ON)

# qmake project file (alternative to CMakeLists.txt)
# Open this file in Qt Creator and press Run.

QT += widgets
CONFIG += c++17
TARGET = HotelBookingGUI
TEMPLATE = app

# The OOP classes are shared with the console version (one folder up)
INCLUDEPATH += ..

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    ../Person.cpp \
    ../Room.cpp \
    ../Hotel.cpp \
    ../Booking.cpp \
    ../InputValidator.cpp \
    ../HotelManager.cpp

HEADERS += \
    MainWindow.h \
    ../Person.h \
    ../Room.h \
    ../Hotel.h \
    ../Booking.h \
    ../InputValidator.h \
    ../SearchUtility.h \
    ../HotelManager.h

// ============================================================
//  gui/main.cpp  -  starts the Qt GUI version
//
//  hotels.txt and guest_info.txt are kept next to the .exe,
//  so the GUI and console versions can share the same data
//  if you copy the files between their folders.
// ============================================================
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <stdexcept>

#include "HotelManager.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Smart Hotel Booking System");

    // keep the data files in the same folder as the program
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    HotelManager manager;
    try {
        manager.load();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Could not load data", QString::fromStdString(e.what()));
        return 1;
    }

    MainWindow window(manager);
    window.show();
    return app.exec();
}

// ============================================================
//  MainWindow.h  (Qt GUI version)
//  Class: MainWindow  (inherits QMainWindow)
//
//  The window only shows data and reads input. All booking
//  logic, validation and file handling come from the same
//  classes the console version uses (HotelManager, Booking,
//  Hotel, Room, Guest, Admin, InputValidator).
// ============================================================
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <string>
#include <vector>

#include "HotelManager.h"
#include "Person.h"

class QListWidget;
class QStackedWidget;
class QComboBox;
class QTableWidget;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QDateEdit;
class QCheckBox;
class QLabel;
class QPushButton;
class QPlainTextEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(HotelManager& manager, QWidget* parent = nullptr);

private slots:
    void refreshDirectory();
    void refreshBookingHotels();
    void onBookingHotelSelected();
    void updateEstimate();
    void onConfirmBooking();
    void onLookupBooking();
    void onFindForCancel();
    void onCancelBooking();
    void onAdminLogin();
    void onAdminLogout();
    void onTariffCityChanged();
    void onTariffHotelChanged();
    void onSaveTariff();

private:
    HotelManager& manager;
    Admin admin;

    // layout
    QListWidget* sidebar;
    QStackedWidget* pages;

    // page 1: hotel directory
    QComboBox* dirCityBox;
    QTableWidget* dirTable;

    // page 2: book a room
    QComboBox* bookCityBox;
    QTableWidget* bookHotelTable;
    std::vector<int> bookHotelIndexes;
    QLabel* selectedHotelLabel;
    QLineEdit* nameEdit;
    QLineEdit* cnicEdit;
    QLineEdit* phoneEdit;
    QSpinBox* guestsSpin;
    QSpinBox* singleSpin;
    QSpinBox* doubleSpin;
    QDateEdit* dateEdit;
    QSpinBox* nightsSpin;
    QCheckBox* sceneryCheck;
    QLabel* capacityLabel;
    QLabel* totalLabel;
    QLabel* bookErrorLabel;

    // page 3: my booking
    QLineEdit* lookupCnicEdit;
    QLabel* lookupErrorLabel;
    QPlainTextEdit* lookupResult;

    // page 4: cancel booking
    QLineEdit* cancelCnicEdit;
    QLabel* cancelErrorLabel;
    QTableWidget* cancelTable;
    std::vector<int> cancelIndexes;
    QPushButton* cancelButton;

    // page 5: admin
    QStackedWidget* adminStack;
    QLineEdit* userEdit;
    QLineEdit* passEdit;
    QLabel* loginErrorLabel;
    int loginAttempts;
    QLabel* adminInfoLabel;
    QLabel* revenueValue;
    QLabel* bookingsValue;
    QLabel* freeSinglesValue;
    QLabel* freeDoublesValue;
    QTableWidget* reservationsTable;
    QTableWidget* revenueTable;
    QComboBox* tariffCityBox;
    QComboBox* tariffHotelBox;
    QDoubleSpinBox* singlePriceSpin;
    QDoubleSpinBox* doublePriceSpin;
    QDoubleSpinBox* sceneryPriceSpin;

    // page builders
    QWidget* buildDirectoryPage();
    QWidget* buildBookingPage();
    QWidget* buildLookupPage();
    QWidget* buildCancelPage();
    QWidget* buildAdminPage();
    QWidget* buildAdminLogin();
    QWidget* buildAdminDashboard();

    // helpers
    void refreshAll();
    void refreshAdmin();
    void resetBookingForm();
    void showReceipt(const QString& title, const QString& text);
    int selectedBookingHotel() const;
    QComboBox* makeCityBox(bool withAll);
    QTableWidget* makeTable(const QStringList& headers);
    QLabel* makeErrorLabel();
    QWidget* makeMetric(const QString& title, QLabel*& valueLabel);

    static QString qs(const std::string& s);
    static std::string ss(const QString& s);
    static QString money(double amount);
};

#endif

// ============================================================
//  main.cpp  (console version)
//  Smart Hotel Booking & Management System
//
//  All hotels, bookings and files are handled by HotelManager.
//  This file only shows menus and reads input.
//  (The Qt GUI version is in the gui/ folder.)
//
//  Compile:  g++ -std=c++17 *.cpp -o HotelBooking
// ============================================================
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "Person.h"
#include "HotelManager.h"
#include "InputValidator.h"
using namespace std;

// ======================= Display helpers ======================

void printHeader(const string& title) {
    cout << "\n  ===================================================\n";
    cout << "   " << title << endl;
    cout << "  ===================================================\n";
}

void pause() {
    InputValidator::readLine("\n  Press Enter to continue...");
}

// Polymorphism through a base-class reference
void showProfile(const Person& p) {
    cout << "  [" << p.getRole() << "]\n";
    p.displayDetails();
}

void saveData(const HotelManager& manager) {
    try {
        manager.save();
    } catch (const runtime_error& e) {
        cout << "  [!] File error: " << e.what() << endl;
    }
}

// Shows the hotels of one city. Returns how many were shown and fills idx[].
int showHotelsOfCity(const HotelManager& manager, const string& city, int idx[]) {
    int found = manager.findHotelsByCity(city, idx, HotelManager::MAX_HOTELS);
    cout << "\n  Hotels in " << city << ":\n";
    cout << "  ----------------------------------------------------------------"
            "------------------------------\n";
    for (int i = 0; i < found; i++)
        manager.getHotel(idx[i]).displayDetails(i + 1);
    cout << "  ----------------------------------------------------------------"
            "------------------------------\n";
    return found;
}

int chooseCity() {
    cout << "\n  Select a city:\n";
    for (int i = 0; i < HotelManager::CITY_COUNT; i++)
        cout << "   " << i + 1 << ". " << HotelManager::CITIES[i] << endl;
    return InputValidator::readInt("  Enter choice (1-8): ", 1, HotelManager::CITY_COUNT) - 1;
}

// ===================== 1. Hotel directory =====================

void viewHotelDirectory(const HotelManager& manager) {
    printHeader("MULTI-CITY HOTEL DIRECTORY");
    cout << "   1. View hotels of one city\n";
    cout << "   2. View all 40 hotels\n";
    int choice = InputValidator::readInt("  Enter choice (1-2): ", 1, 2);

    int idx[HotelManager::MAX_HOTELS];
    if (choice == 1) {
        int c = chooseCity();
        showHotelsOfCity(manager, HotelManager::CITIES[c], idx);
    } else {
        for (int c = 0; c < HotelManager::CITY_COUNT; c++)
            showHotelsOfCity(manager, HotelManager::CITIES[c], idx);
    }
    pause();
}

// ===================== 2. Book a room =========================

void bookRoom(HotelManager& manager) {
    printHeader("ROOM RESERVATION");

    // Step 1: choose city and hotel
    int c = chooseCity();
    int idx[HotelManager::MAX_HOTELS];
    int found = showHotelsOfCity(manager, HotelManager::CITIES[c], idx);
    if (found == 0) {
        cout << "  No hotels found in this city.\n";
        pause();
        return;
    }
    int h = InputValidator::readInt("  Select hotel (1-" + to_string(found) + "): ",
                                    1, found) - 1;
    int hotelIndex = idx[h];
    const Hotel& hotel = manager.getHotel(hotelIndex);

    // Step 2: guest details
    cout << "\n  Enter guest details:\n";
    string name  = InputValidator::readValid("  Full name               : ",
                                             InputValidator::validateName);
    string cnic  = InputValidator::readValid("  CNIC (00000-0000000-0)  : ",
                                             InputValidator::validateCNIC);
    string phone = InputValidator::readValid("  Phone (0000-0000000)    : ",
                                             InputValidator::validatePhone);
    int guests   = InputValidator::readInt  ("  Number of guests (1-20) : ", 1, 20);

    // Step 3: room details
    cout << "\n  Available at " << hotel.getName() << ": "
         << hotel.getSingle()->getAvailable() << " single, "
         << hotel.getDouble()->getAvailable() << " double\n";
    int singles = InputValidator::readInt("  Single rooms to book    : ", 0,
                                          hotel.getSingle()->getAvailable());
    int doubles = InputValidator::readInt("  Double rooms to book    : ", 0,
                                          hotel.getDouble()->getAvailable());
    string date = InputValidator::readValid("  Check-in (DD-MM-YYYY)   : ",
                                            InputValidator::validateDate);
    int nights  = InputValidator::readInt  ("  Number of nights (1-30) : ", 1, 30);

    cout << fixed << setprecision(0);
    bool scenery = InputValidator::readYesNo("  Add scenic view for PKR " +
        to_string((int)hotel.getSceneryPrice()) + "/night? (y/n): ");

    cout << "\n  Estimated total: PKR "
         << manager.estimateTotal(hotelIndex, singles, doubles, nights, scenery) << endl;
    if (!InputValidator::readYesNo("  Confirm this booking? (y/n): ")) {
        cout << "  Booking cancelled by user.\n";
        pause();
        return;
    }

    // Step 4: create and confirm booking (exceptions handled here)
    try {
        Guest guest(name, cnic, phone, guests);
        manager.addBooking(guest, hotelIndex, singles, doubles, nights, date, scenery);
        cout << "\n  Booking confirmed successfully!\n\n";
        manager.getBooking(manager.getBookingCount() - 1).printReceipt();
    } catch (const invalid_argument& e) {
        cout << "  [!] Invalid booking: " << e.what() << endl;
    } catch (const runtime_error& e) {
        cout << "  [!] Booking failed: " << e.what() << endl;
    }
    pause();
}

// ===================== 3. Booking lookup ======================

void lookupBooking(const HotelManager& manager) {
    printHeader("BOOKING LOOKUP");
    string cnic = InputValidator::readValid("  Enter CNIC (00000-0000000-0): ",
                                            InputValidator::validateCNIC);

    int idx[HotelManager::MAX_BOOKINGS];
    int found = manager.findBookingsByCnic(cnic, idx, HotelManager::MAX_BOOKINGS);
    if (found == 0) {
        cout << "  No booking found for CNIC " << cnic << ".\n";
    } else {
        cout << "  " << found << " booking(s) found:\n\n";
        for (int i = 0; i < found; i++) {
            cout << "  Booking #" << i + 1 << endl;
            manager.getBooking(idx[i]).printReceipt();
            cout << endl;
        }
    }
    pause();
}

// ===================== 4. Cancel booking ======================

void cancelBooking(HotelManager& manager) {
    printHeader("CANCEL BOOKING");
    string cnic = InputValidator::readValid("  Enter CNIC (00000-0000000-0): ",
                                            InputValidator::validateCNIC);

    int idx[HotelManager::MAX_BOOKINGS];
    int found = manager.findBookingsByCnic(cnic, idx, HotelManager::MAX_BOOKINGS);
    if (found == 0) {
        cout << "  No booking found for CNIC " << cnic << ".\n";
        pause();
        return;
    }

    cout << fixed << setprecision(0);
    for (int i = 0; i < found; i++) {
        const Booking& b = manager.getBooking(idx[i]);
        cout << "   " << i + 1 << ". " << b.getHotel()->getName()
             << " - PKR " << b.getTotal() << endl;
    }
    int choice = InputValidator::readInt("  Which booking to cancel (1-" +
                                         to_string(found) + "): ", 1, found) - 1;

    if (!InputValidator::readYesNo("  Are you sure? (y/n): ")) {
        cout << "  Nothing was cancelled.\n";
        pause();
        return;
    }

    try {
        manager.cancelBooking(idx[choice]);
        cout << "  Booking cancelled. Rooms have been released.\n";
    } catch (const exception& e) {
        cout << "  [!] " << e.what() << endl;
    }
    pause();
}

// ===================== 5. Admin portal ========================

void adminViewAll(const HotelManager& manager) {
    printHeader("ALL ACTIVE RESERVATIONS");
    if (manager.getBookingCount() == 0) {
        cout << "  There are no active bookings.\n";
        return;
    }
    cout << fixed << setprecision(0);
    cout << "  " << left << setw(4) << "No" << setw(20) << "Guest" << setw(17) << "CNIC"
         << setw(30) << "Hotel" << right << setw(10) << "Total" << endl;
    cout << "  ----------------------------------------------------------------"
            "---------------\n";
    for (int i = 0; i < manager.getBookingCount(); i++) {
        const Booking& b = manager.getBooking(i);
        cout << "  " << left << setw(4) << i + 1
             << setw(20) << b.getGuest().getName().substr(0, 19)
             << setw(17) << b.getGuest().getCnic()
             << setw(30) << b.getHotel()->getName().substr(0, 29)
             << right << setw(10) << b.getTotal() << endl;
    }
}

void adminChangeTariff(HotelManager& manager) {
    printHeader("ADJUST ROOM TARIFFS");
    int c = chooseCity();
    int idx[HotelManager::MAX_HOTELS];
    int found = showHotelsOfCity(manager, HotelManager::CITIES[c], idx);
    if (found == 0) return;

    int h = InputValidator::readInt("  Select hotel (1-" + to_string(found) + "): ",
                                    1, found) - 1;
    Hotel& hotel = manager.getHotel(idx[h]);

    cout << "   1. Single room price\n   2. Double room price\n   3. Scenic view price\n";
    int what = InputValidator::readInt("  What to change (1-3): ", 1, 3);
    double price = InputValidator::readDouble("  New price in PKR: ", 0, 500000);

    try {
        if (what == 1)      hotel.getSingle()->setPrice(price);
        else if (what == 2) hotel.getDouble()->setPrice(price);
        else                hotel.setSceneryPrice(price);
        saveData(manager);
        cout << "  Tariff updated for " << hotel.getName() << ".\n";
    } catch (const invalid_argument& e) {
        cout << "  [!] " << e.what() << endl;
    }
}

void adminRevenue(const HotelManager& manager) {
    printHeader("REVENUE SUMMARY");
    cout << fixed << setprecision(0);
    for (int c = 0; c < HotelManager::CITY_COUNT; c++)
        cout << "  " << left << setw(12) << HotelManager::CITIES[c] << right
             << " PKR " << setw(12) << manager.revenueForCity(HotelManager::CITIES[c]) << endl;
    cout << "  ---------------------------------\n";
    cout << "  " << left << setw(12) << "TOTAL" << right
         << " PKR " << setw(12) << manager.totalRevenue() << endl;
    cout << "  Active bookings: " << Booking::getActiveBookings() << endl;
    cout << "  Rooms free now : " << manager.freeSingleRooms() << " single, "
         << manager.freeDoubleRooms() << " double\n";
}

void adminPortal(HotelManager& manager) {
    printHeader("ADMINISTRATIVE PORTAL - LOGIN");
    Admin admin("Hotel Manager", "admin", "admin123");

    int attempts = 0;
    bool loggedIn = false;
    while (attempts < 3 && !loggedIn) {
        string user = InputValidator::readLine("  Username: ");
        string pass = InputValidator::readLine("  Password: ");
        if (admin.login(user, pass)) {
            loggedIn = true;
        } else {
            attempts++;
            cout << "  [!] Wrong username or password ("
                 << 3 - attempts << " attempt(s) left).\n";
        }
    }
    if (!loggedIn) {
        cout << "  Access denied.\n";
        pause();
        return;
    }

    cout << endl;
    showProfile(admin);     // polymorphic call through Person&

    while (true) {
        printHeader("ADMIN MENU");
        cout << "   1. View all active reservations\n";
        cout << "   2. Adjust room tariffs\n";
        cout << "   3. View revenue summary\n";
        cout << "   4. Logout\n";
        int choice = InputValidator::readInt("  Enter choice (1-4): ", 1, 4);

        if (choice == 1)      adminViewAll(manager);
        else if (choice == 2) adminChangeTariff(manager);
        else if (choice == 3) adminRevenue(manager);
        else break;
        pause();
    }
}

// ============================ main ============================

int main() {
    HotelManager manager;

    try {
        manager.load();
        if (manager.isFirstRun())
            cout << "  (First run: " << HotelManager::HOTEL_FILE
                 << " created with 40 hotels.)\n";
    } catch (const runtime_error& e) {
        cout << "  [!] Fatal error: " << e.what() << endl;
        return 1;
    }

    while (true) {
        printHeader("SMART HOTEL BOOKING & MANAGEMENT SYSTEM");
        cout << "   Hotels: " << manager.getHotelCount() << " in "
             << HotelManager::CITY_COUNT << " cities"
             << "   |   Active bookings: " << Booking::getActiveBookings() << endl;
        cout << "  ---------------------------------------------------\n";
        cout << "   1. View hotel directory\n";
        cout << "   2. Book a room\n";
        cout << "   3. View my booking (by CNIC)\n";
        cout << "   4. Cancel a booking\n";
        cout << "   5. Admin portal\n";
        cout << "   6. Exit\n";
        int choice = InputValidator::readInt("  Enter choice (1-6): ", 1, 6);

        switch (choice) {
            case 1: viewHotelDirectory(manager); break;
            case 2: bookRoom(manager); break;
            case 3: lookupBooking(manager); break;
            case 4: cancelBooking(manager); break;
            case 5: adminPortal(manager); break;
            case 6:
                saveData(manager);
                cout << "\n  Data saved. Thank you for using the system. Goodbye!\n";
                return 0;
        }
    }
}

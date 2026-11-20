// ============================================================
//  main.cpp
//  Smart Hotel Booking & Management System
//  CSL-210 Object-Oriented Programming Lab - Semester Project
//
//  Files used for permanent storage:
//    hotels.txt      -> hotel list, tariffs, room availability
//    guest_info.txt  -> all active guest bookings
//
//  Compile:  g++ -std=c++17 *.cpp -o HotelBooking
// ============================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "Person.h"
#include "Room.h"
#include "Hotel.h"
#include "Booking.h"
#include "InputValidator.h"
#include "SearchUtility.h"
using namespace std;

const int MAX_HOTELS = 40;
const int MAX_BOOKINGS = 500;
const string HOTEL_FILE = "hotels.txt";
const string BOOKING_FILE = "guest_info.txt";

const int CITY_COUNT = 8;
const string CITIES[CITY_COUNT] = {"Lahore", "Karachi", "Islamabad", "Rawalpindi",
                                   "Peshawar", "Multan", "Hyderabad", "Swat"};

// ------------------------------------------------------------
//  Default data (written to hotels.txt on the very first run)
//  name|city|singleAvail|singlePrice|doubleAvail|doublePrice|scenery
// ------------------------------------------------------------
const string DEFAULT_HOTELS[MAX_HOTELS] = {
    "Pearl Continental Lahore|Lahore|10|18000|8|26000|2500",
    "Avari Lahore|Lahore|12|16000|10|24000|2000",
    "Faletti's Hotel|Lahore|8|14000|6|21000|1500",
    "Nishat Hotel Johar Town|Lahore|10|15000|8|22000|2000",
    "Hospitality Inn Lahore|Lahore|15|9000|10|14000|1500",

    "Pearl Continental Karachi|Karachi|10|19000|8|27000|3000",
    "Avari Towers Karachi|Karachi|12|17000|10|25000|2500",
    "Movenpick Karachi|Karachi|10|20000|8|29000|3000",
    "Beach Luxury Hotel|Karachi|8|12000|6|18000|3000",
    "Hotel One Karachi|Karachi|15|8000|10|12000|1500",

    "Serena Islamabad|Islamabad|10|25000|8|35000|3000",
    "Marriott Islamabad|Islamabad|12|22000|10|31000|2500",
    "Ramada Islamabad|Islamabad|10|14000|8|20000|2000",
    "Margala Hotel|Islamabad|12|10000|8|15000|2500",
    "Best Western Islamabad|Islamabad|10|11000|8|16000|1500",

    "Pearl Continental Rawalpindi|Rawalpindi|10|16000|8|23000|1500",
    "Hotel One Rawalpindi|Rawalpindi|12|7000|8|11000|1500",
    "Grand Regency Rawalpindi|Rawalpindi|10|9000|8|13000|1500",
    "Shalimar Hotel|Rawalpindi|8|8000|6|12000|1500",
    "Rawal Inn|Rawalpindi|15|5000|10|8000|1500",

    "Pearl Continental Peshawar|Peshawar|10|15000|8|22000|2000",
    "Shelton Rezidor Peshawar|Peshawar|8|10000|6|15000|1500",
    "Khyber Inn|Peshawar|12|6000|8|9500|1500",
    "Green's Hotel Peshawar|Peshawar|10|7000|8|10500|1500",
    "Hotel Grand Peshawar|Peshawar|10|6500|8|10000|1500",

    "Ramada Multan|Multan|10|13000|8|19000|2000",
    "Hotel One Multan|Multan|12|7000|8|11000|1500",
    "Sindbad Hotel Multan|Multan|10|6000|8|9000|1500",
    "Shelton Multan|Multan|8|8000|6|12000|1500",
    "Multan Continental|Multan|10|7500|8|11500|1500",

    "Indus Hotel Hyderabad|Hyderabad|10|8000|8|12000|2000",
    "City Gate Hotel|Hyderabad|10|6000|8|9000|1500",
    "Hotel One Hyderabad|Hyderabad|12|7000|8|10500|1500",
    "Faran Hotel Hyderabad|Hyderabad|8|5500|6|8500|1500",
    "Sindh Inn Hyderabad|Hyderabad|10|5000|8|7500|1500",

    "Serena Swat|Swat|10|18000|8|26000|3000",
    "PC Malam Jabba|Swat|8|20000|6|28000|3000",
    "Swat Continental|Swat|10|9000|8|13000|2500",
    "Rock City Resort|Swat|10|11000|8|16000|3000",
    "Hotel One Swat|Swat|12|8000|8|12000|2500"
};

// ======================= File handling ========================

// Splits "a|b|c" into parts[], returns how many parts were found
int splitLine(const string& line, string parts[], int maxParts) {
    stringstream ss(line);
    int count = 0;
    while (count < maxParts && getline(ss, parts[count], '|'))
        count++;
    return count;
}

void createDefaultHotelFile() {
    ofstream out(HOTEL_FILE);
    if (!out) throw runtime_error("Cannot create " + HOTEL_FILE);
    for (int i = 0; i < MAX_HOTELS; i++)
        out << DEFAULT_HOTELS[i] << endl;
    out.close();
    cout << "  (First run: " << HOTEL_FILE << " created with 40 hotels.)\n";
}

int loadHotels(Hotel hotels[]) {
    ifstream in(HOTEL_FILE);
    if (!in) {
        createDefaultHotelFile();
        in.open(HOTEL_FILE);
        if (!in) throw runtime_error("Cannot open " + HOTEL_FILE);
    }

    int count = 0;
    string line;
    while (count < MAX_HOTELS && getline(in, line)) {
        string p[7];
        if (splitLine(line, p, 7) != 7) continue;   // skip broken lines
        try {
            hotels[count].setData(p[0], p[1], stoi(p[2]), stod(p[3]),
                                  stoi(p[4]), stod(p[5]), stod(p[6]));
            count++;
        } catch (const exception&) {
            cout << "  [!] Skipped a damaged line in " << HOTEL_FILE << endl;
        }
    }
    in.close();
    return count;
}

void saveHotels(const Hotel hotels[], int count) {
    ofstream out(HOTEL_FILE);
    if (!out) throw runtime_error("Cannot write " + HOTEL_FILE);
    for (int i = 0; i < count; i++)
        out << hotels[i].toFileString() << endl;
    out.close();
}

int loadBookings(Booking bookings[], Hotel hotels[], int hotelCount) {
    ifstream in(BOOKING_FILE);
    if (!in) return 0;     // no bookings yet

    int count = 0;
    string line;
    while (count < MAX_BOOKINGS && getline(in, line)) {
        if (line.empty()) continue;
        if (Booking::fromFileString(line, hotels, hotelCount, bookings[count]))
            count++;
    }
    in.close();
    return count;
}

void saveBookings(const Booking bookings[], int count) {
    ofstream out(BOOKING_FILE);
    if (!out) throw runtime_error("Cannot write " + BOOKING_FILE);
    for (int i = 0; i < count; i++)
        out << bookings[i].toFileString() << endl;
    out.close();
}

void saveAll(const Hotel hotels[], int hotelCount,
             const Booking bookings[], int bookingCount) {
    try {
        saveHotels(hotels, hotelCount);
        saveBookings(bookings, bookingCount);
    } catch (const runtime_error& e) {
        cout << "  [!] File error: " << e.what() << endl;
    }
}

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

// Shows the hotels of one city. Returns how many were shown and fills idx[].
int showHotelsOfCity(const Hotel hotels[], int hotelCount,
                     const string& city, int idx[]) {
    int found = SearchUtility<Hotel>::findAll(hotels, hotelCount, city, idx, MAX_HOTELS);
    cout << "\n  Hotels in " << city << ":\n";
    cout << "  ----------------------------------------------------------------"
            "------------------------------\n";
    for (int i = 0; i < found; i++)
        hotels[idx[i]].displayDetails(i + 1);
    cout << "  ----------------------------------------------------------------"
            "------------------------------\n";
    return found;
}

int chooseCity() {
    cout << "\n  Select a city:\n";
    for (int i = 0; i < CITY_COUNT; i++)
        cout << "   " << i + 1 << ". " << CITIES[i] << endl;
    return InputValidator::readInt("  Enter choice (1-8): ", 1, CITY_COUNT) - 1;
}

// ===================== 1. Hotel directory =====================

void viewHotelDirectory(const Hotel hotels[], int hotelCount) {
    printHeader("MULTI-CITY HOTEL DIRECTORY");
    cout << "   1. View hotels of one city\n";
    cout << "   2. View all 40 hotels\n";
    int choice = InputValidator::readInt("  Enter choice (1-2): ", 1, 2);

    int idx[MAX_HOTELS];
    if (choice == 1) {
        int c = chooseCity();
        showHotelsOfCity(hotels, hotelCount, CITIES[c], idx);
    } else {
        for (int c = 0; c < CITY_COUNT; c++)
            showHotelsOfCity(hotels, hotelCount, CITIES[c], idx);
    }
    pause();
}

// ===================== 2. Book a room =========================

void bookRoom(Hotel hotels[], int hotelCount,
              Booking bookings[], int& bookingCount) {
    printHeader("ROOM RESERVATION");

    if (bookingCount >= MAX_BOOKINGS) {
        cout << "  [!] Booking storage is full. Please contact the admin.\n";
        pause();
        return;
    }

    // Step 1: choose city and hotel
    int c = chooseCity();
    int idx[MAX_HOTELS];
    int found = showHotelsOfCity(hotels, hotelCount, CITIES[c], idx);
    if (found == 0) {
        cout << "  No hotels found in this city.\n";
        pause();
        return;
    }
    int h = InputValidator::readInt("  Select hotel (1-" + to_string(found) + "): ",
                                    1, found) - 1;
    int hotelIndex = idx[h];
    Hotel* hotel = &hotels[hotelIndex];

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
    cout << "\n  Available at " << hotel->getName() << ": "
         << hotel->getSingle()->getAvailable() << " single, "
         << hotel->getDouble()->getAvailable() << " double\n";
    int singles = InputValidator::readInt("  Single rooms to book    : ", 0,
                                          hotel->getSingle()->getAvailable());
    int doubles = InputValidator::readInt("  Double rooms to book    : ", 0,
                                          hotel->getDouble()->getAvailable());
    string date = InputValidator::readValid("  Check-in (DD-MM-YYYY)   : ",
                                            InputValidator::validateDate);
    int nights  = InputValidator::readInt  ("  Number of nights (1-30) : ", 1, 30);

    cout << fixed << setprecision(0);
    bool scenery = InputValidator::readYesNo("  Add scenic view for PKR " +
        to_string((int)hotel->getSceneryPrice()) + "/night? (y/n): ");

    // Step 4: create and confirm booking (exceptions handled here)
    Guest guest(name, cnic, phone, guests);
    Booking booking(guest, hotel, hotelIndex, singles, doubles, nights, date, scenery);

    cout << "\n  Estimated total: PKR " << booking.calculateTotal() << endl;
    if (!InputValidator::readYesNo("  Confirm this booking? (y/n): ")) {
        cout << "  Booking cancelled by user.\n";
        pause();
        return;
    }

    try {
        booking.confirm();
        bookings[bookingCount++] = booking;
        saveAll(hotels, hotelCount, bookings, bookingCount);
        cout << "\n  Booking confirmed successfully!\n\n";
        booking.printReceipt();
    } catch (const invalid_argument& e) {
        cout << "  [!] Invalid booking: " << e.what() << endl;
    } catch (const runtime_error& e) {
        cout << "  [!] Booking failed: " << e.what() << endl;
    }
    pause();
}

// ===================== 3. Booking lookup ======================

void lookupBooking(const Booking bookings[], int bookingCount) {
    printHeader("BOOKING LOOKUP");
    string cnic = InputValidator::readValid("  Enter CNIC (00000-0000000-0): ",
                                            InputValidator::validateCNIC);

    int idx[MAX_BOOKINGS];
    int found = SearchUtility<Booking>::findAll(bookings, bookingCount, cnic,
                                                idx, MAX_BOOKINGS);
    if (found == 0) {
        cout << "  No booking found for CNIC " << cnic << ".\n";
    } else {
        cout << "  " << found << " booking(s) found:\n\n";
        for (int i = 0; i < found; i++) {
            cout << "  Booking #" << i + 1 << endl;
            bookings[idx[i]].printReceipt();
            cout << endl;
        }
    }
    pause();
}

// ===================== 4. Cancel booking ======================

void cancelBooking(Hotel hotels[], int hotelCount,
                   Booking bookings[], int& bookingCount) {
    printHeader("CANCEL BOOKING");
    string cnic = InputValidator::readValid("  Enter CNIC (00000-0000000-0): ",
                                            InputValidator::validateCNIC);

    int idx[MAX_BOOKINGS];
    int found = SearchUtility<Booking>::findAll(bookings, bookingCount, cnic,
                                                idx, MAX_BOOKINGS);
    if (found == 0) {
        cout << "  No booking found for CNIC " << cnic << ".\n";
        pause();
        return;
    }

    cout << fixed << setprecision(0);
    for (int i = 0; i < found; i++) {
        const Booking& b = bookings[idx[i]];
        cout << "   " << i + 1 << ". " << b.getHotel()->getName()
             << " - PKR " << b.getTotal() << endl;
    }
    int choice = InputValidator::readInt("  Which booking to cancel (1-" +
                                         to_string(found) + "): ", 1, found) - 1;
    int pos = idx[choice];

    if (!InputValidator::readYesNo("  Are you sure? (y/n): ")) {
        cout << "  Nothing was cancelled.\n";
        pause();
        return;
    }

    bookings[pos].cancel();               // rooms restored with ++
    for (int i = pos; i < bookingCount - 1; i++)
        bookings[i] = bookings[i + 1];    // remove from array
    bookingCount--;
    Booking::decreaseActiveBookings();

    saveAll(hotels, hotelCount, bookings, bookingCount);
    cout << "  Booking cancelled. Rooms have been released.\n";
    pause();
}

// ===================== 5. Admin portal ========================

void adminViewAll(const Booking bookings[], int bookingCount) {
    printHeader("ALL ACTIVE RESERVATIONS");
    if (bookingCount == 0) {
        cout << "  There are no active bookings.\n";
        return;
    }
    cout << fixed << setprecision(0);
    cout << "  " << left << setw(4) << "No" << setw(20) << "Guest" << setw(17) << "CNIC"
         << setw(30) << "Hotel" << right << setw(10) << "Total" << endl;
    cout << "  ----------------------------------------------------------------"
            "---------------\n";
    for (int i = 0; i < bookingCount; i++) {
        const Booking& b = bookings[i];
        cout << "  " << left << setw(4) << i + 1
             << setw(20) << b.getGuest().getName().substr(0, 19)
             << setw(17) << b.getGuest().getCnic()
             << setw(30) << b.getHotel()->getName().substr(0, 29)
             << right << setw(10) << b.getTotal() << endl;
    }
}

void adminChangeTariff(Hotel hotels[], int hotelCount,
                       const Booking bookings[], int bookingCount) {
    printHeader("ADJUST ROOM TARIFFS");
    int c = chooseCity();
    int idx[MAX_HOTELS];
    int found = showHotelsOfCity(hotels, hotelCount, CITIES[c], idx);
    if (found == 0) return;

    int h = InputValidator::readInt("  Select hotel (1-" + to_string(found) + "): ",
                                    1, found) - 1;
    Hotel& hotel = hotels[idx[h]];

    cout << "   1. Single room price\n   2. Double room price\n   3. Scenic view price\n";
    int what = InputValidator::readInt("  What to change (1-3): ", 1, 3);
    double price = InputValidator::readDouble("  New price in PKR: ", 0, 500000);

    try {
        if (what == 1)      hotel.getSingle()->setPrice(price);
        else if (what == 2) hotel.getDouble()->setPrice(price);
        else                hotel.setSceneryPrice(price);
        saveAll(hotels, hotelCount, bookings, bookingCount);
        cout << "  Tariff updated for " << hotel.getName() << ".\n";
    } catch (const invalid_argument& e) {
        cout << "  [!] " << e.what() << endl;
    }
}

void adminRevenue(const Hotel hotels[], int hotelCount,
                  const Booking bookings[], int bookingCount) {
    printHeader("REVENUE SUMMARY");
    double cityTotal[CITY_COUNT] = {0};
    double grandTotal = 0;

    for (int i = 0; i < bookingCount; i++) {
        grandTotal += bookings[i].getTotal();
        for (int c = 0; c < CITY_COUNT; c++) {
            if (bookings[i].getHotel()->matches(CITIES[c]))
                cityTotal[c] += bookings[i].getTotal();
        }
    }

    cout << fixed << setprecision(0);
    for (int c = 0; c < CITY_COUNT; c++)
        cout << "  " << left << setw(12) << CITIES[c] << right
             << " PKR " << setw(12) << cityTotal[c] << endl;
    cout << "  ---------------------------------\n";
    cout << "  " << left << setw(12) << "TOTAL" << right
         << " PKR " << setw(12) << grandTotal << endl;
    cout << "  Active bookings: " << Booking::getActiveBookings() << endl;

    int freeSingles = 0, freeDoubles = 0;
    for (int i = 0; i < hotelCount; i++) {
        freeSingles += hotels[i].getSingle()->getAvailable();
        freeDoubles += hotels[i].getDouble()->getAvailable();
    }
    cout << "  Rooms free now : " << freeSingles << " single, "
         << freeDoubles << " double\n";
}

void adminPortal(Hotel hotels[], int hotelCount,
                 Booking bookings[], int& bookingCount) {
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

        if (choice == 1)      adminViewAll(bookings, bookingCount);
        else if (choice == 2) adminChangeTariff(hotels, hotelCount, bookings, bookingCount);
        else if (choice == 3) adminRevenue(hotels, hotelCount, bookings, bookingCount);
        else break;
        pause();
    }
}

// ============================ main ============================

int main() {
    // Arrays of objects
    Hotel* hotels = new Hotel[MAX_HOTELS];
    Booking* bookings = new Booking[MAX_BOOKINGS];
    int hotelCount = 0;
    int bookingCount = 0;

    try {
        hotelCount = loadHotels(hotels);
        bookingCount = loadBookings(bookings, hotels, hotelCount);
    } catch (const runtime_error& e) {
        cout << "  [!] Fatal error: " << e.what() << endl;
        delete[] hotels;
        delete[] bookings;
        return 1;
    }

    while (true) {
        printHeader("SMART HOTEL BOOKING & MANAGEMENT SYSTEM");
        cout << "   Hotels: " << hotelCount << " in " << CITY_COUNT << " cities"
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
            case 1: viewHotelDirectory(hotels, hotelCount); break;
            case 2: bookRoom(hotels, hotelCount, bookings, bookingCount); break;
            case 3: lookupBooking(bookings, bookingCount); break;
            case 4: cancelBooking(hotels, hotelCount, bookings, bookingCount); break;
            case 5: adminPortal(hotels, hotelCount, bookings, bookingCount); break;
            case 6:
                saveAll(hotels, hotelCount, bookings, bookingCount);
                cout << "\n  Data saved. Thank you for using the system. Goodbye!\n";
                delete[] hotels;
                delete[] bookings;
                return 0;
        }
    }
}

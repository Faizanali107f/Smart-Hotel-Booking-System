// ============================================================
//  HotelManager.cpp  -  implementation of HotelManager
// ============================================================
#include "HotelManager.h"
#include "SearchUtility.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
using namespace std;

const string HotelManager::CITIES[HotelManager::CITY_COUNT] = {
    "Lahore", "Karachi", "Islamabad", "Rawalpindi",
    "Peshawar", "Multan", "Hyderabad", "Swat"};
const string HotelManager::HOTEL_FILE = "hotels.txt";
const string HotelManager::BOOKING_FILE = "guest_info.txt";

// ------------------------------------------------------------
//  Default data (written to hotels.txt on the very first run)
//  name|city|singleAvail|singlePrice|doubleAvail|doublePrice|scenery
// ------------------------------------------------------------
static const string DEFAULT_HOTELS[HotelManager::MAX_HOTELS] = {
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

// Splits "a|b|c" into parts[], returns how many parts were found
static int splitLine(const string& line, string parts[], int maxParts) {
    stringstream ss(line);
    int count = 0;
    while (count < maxParts && getline(ss, parts[count], '|'))
        count++;
    return count;
}

// ======================= constructor / destructor ===================
HotelManager::HotelManager()
    : hotels(new Hotel[MAX_HOTELS]), bookings(new Booking[MAX_BOOKINGS]),
      hotelCount(0), bookingCount(0), createdDefaultData(false) {}

HotelManager::~HotelManager() {
    delete[] hotels;
    delete[] bookings;
}

// =========================== file handling ==========================
void HotelManager::createDefaultHotelFile() {
    ofstream out(HOTEL_FILE);
    if (!out) throw runtime_error("Cannot create " + HOTEL_FILE);
    for (int i = 0; i < MAX_HOTELS; i++)
        out << DEFAULT_HOTELS[i] << endl;
    createdDefaultData = true;
}

void HotelManager::load() {
    // ---- hotels.txt ----
    ifstream in(HOTEL_FILE);
    if (!in) {
        createDefaultHotelFile();
        in.open(HOTEL_FILE);
        if (!in) throw runtime_error("Cannot open " + HOTEL_FILE);
    }
    hotelCount = 0;
    string line;
    while (hotelCount < MAX_HOTELS && getline(in, line)) {
        string p[7];
        if (splitLine(line, p, 7) != 7) continue;      // skip broken lines
        try {
            hotels[hotelCount].setData(p[0], p[1], stoi(p[2]), stod(p[3]),
                                       stoi(p[4]), stod(p[5]), stod(p[6]));
            hotelCount++;
        } catch (const exception&) {
            // damaged line: ignore it and keep loading the rest
        }
    }
    in.close();
    if (hotelCount == 0)
        throw runtime_error(HOTEL_FILE + " has no valid hotels.");

    // ---- guest_info.txt ----
    bookingCount = 0;
    ifstream bin(BOOKING_FILE);
    if (!bin) return;                                   // no bookings yet
    while (bookingCount < MAX_BOOKINGS && getline(bin, line)) {
        if (line.empty()) continue;
        if (Booking::fromFileString(line, hotels, hotelCount, bookings[bookingCount]))
            bookingCount++;
    }
}

void HotelManager::save() const {
    ofstream hout(HOTEL_FILE);
    if (!hout) throw runtime_error("Cannot write " + HOTEL_FILE);
    for (int i = 0; i < hotelCount; i++)
        hout << hotels[i].toFileString() << endl;

    ofstream bout(BOOKING_FILE);
    if (!bout) throw runtime_error("Cannot write " + BOOKING_FILE);
    for (int i = 0; i < bookingCount; i++)
        bout << bookings[i].toFileString() << endl;
}

bool HotelManager::isFirstRun() const { return createdDefaultData; }

// ============================= getters ==============================
int HotelManager::getHotelCount() const { return hotelCount; }
int HotelManager::getBookingCount() const { return bookingCount; }

Hotel& HotelManager::getHotel(int index) {
    if (index < 0 || index >= hotelCount) throw out_of_range("Invalid hotel number.");
    return hotels[index];
}

const Hotel& HotelManager::getHotel(int index) const {
    if (index < 0 || index >= hotelCount) throw out_of_range("Invalid hotel number.");
    return hotels[index];
}

const Booking& HotelManager::getBooking(int index) const {
    if (index < 0 || index >= bookingCount) throw out_of_range("Invalid booking number.");
    return bookings[index];
}

// ============================= searches =============================
int HotelManager::findHotelsByCity(const string& city, int results[], int maxResults) const {
    return SearchUtility<Hotel>::findAll(hotels, hotelCount, city, results, maxResults);
}

int HotelManager::findBookingsByCnic(const string& cnic, int results[], int maxResults) const {
    return SearchUtility<Booking>::findAll(bookings, bookingCount, cnic, results, maxResults);
}

// ===================== booking and cancellation =====================
void HotelManager::addBooking(const Guest& guest, int hotelIndex, int singles, int doubles,
                              int nights, const string& checkIn, bool scenery) {
    if (bookingCount >= MAX_BOOKINGS)
        throw runtime_error("Booking storage is full. Please contact the admin.");

    Booking booking(guest, &getHotel(hotelIndex), hotelIndex, singles, doubles,
                    nights, checkIn, scenery);
    booking.confirm();                  // throws if rooms/capacity are not enough
    bookings[bookingCount++] = booking;
    save();
}

void HotelManager::cancelBooking(int bookingIndex) {
    if (bookingIndex < 0 || bookingIndex >= bookingCount)
        throw out_of_range("Invalid booking number.");

    bookings[bookingIndex].cancel();    // rooms restored with ++
    for (int i = bookingIndex; i < bookingCount - 1; i++)
        bookings[i] = bookings[i + 1];  // remove from the array
    bookingCount--;
    Booking::decreaseActiveBookings();
    save();
}

double HotelManager::estimateTotal(int hotelIndex, int singles, int doubles,
                                   int nights, bool scenery) const {
    if (hotelIndex < 0 || hotelIndex >= hotelCount) return 0;
    Booking temp(Guest(), &hotels[hotelIndex], hotelIndex, singles, doubles,
                 nights, "", scenery);
    return temp.calculateTotal();
}

// ============================== reports =============================
double HotelManager::revenueForCity(const string& city) const {
    double sum = 0;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].getHotel()->matches(city))
            sum += bookings[i].getTotal();
    }
    return sum;
}

double HotelManager::totalRevenue() const {
    double sum = 0;
    for (int i = 0; i < bookingCount; i++)
        sum += bookings[i].getTotal();
    return sum;
}

int HotelManager::freeSingleRooms() const {
    int sum = 0;
    for (int i = 0; i < hotelCount; i++)
        sum += hotels[i].getSingle()->getAvailable();
    return sum;
}

int HotelManager::freeDoubleRooms() const {
    int sum = 0;
    for (int i = 0; i < hotelCount; i++)
        sum += hotels[i].getDouble()->getAvailable();
    return sum;
}

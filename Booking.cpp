// ============================================================
//  Booking.cpp  -  implementation of Booking
// ============================================================
#include "Booking.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
using namespace std;

// static member definition
int Booking::activeBookings = 0;

Booking::Booking()
    : guest(), hotel(nullptr), hotelIndex(-1), singles(0), doubles(0),
      nights(0), checkIn(""), scenery(false), total(0) {}

Booking::Booking(const Guest& guest, Hotel* hotel, int hotelIndex,
                 int singles, int doubles, int nights,
                 const string& checkIn, bool scenery)
    : guest(guest), hotel(hotel), hotelIndex(hotelIndex), singles(singles),
      doubles(doubles), nights(nights), checkIn(checkIn),
      scenery(scenery), total(0) {}

// Total = single rooms + double rooms (+ scenic view per night)
double Booking::calculateTotal() const {
    if (hotel == nullptr) return 0;
    double sum = 0;
    sum += hotel->getSingle()->calculateCost(singles, nights);   // polymorphism
    sum += hotel->getDouble()->calculateCost(doubles, nights);   // polymorphism
    if (scenery)
        sum += hotel->getSceneryPrice() * nights;
    return sum;
}

// Books the rooms. Throws an exception if the request is not possible.
void Booking::confirm() {
    if (hotel == nullptr)
        throw runtime_error("No hotel selected.");
    if (singles + doubles == 0)
        throw invalid_argument("You must book at least one room.");
    if (singles > hotel->getSingle()->getAvailable())
        throw runtime_error("Overbooking: only " +
            to_string(hotel->getSingle()->getAvailable()) + " single room(s) available.");
    if (doubles > hotel->getDouble()->getAvailable())
        throw runtime_error("Overbooking: only " +
            to_string(hotel->getDouble()->getAvailable()) + " double room(s) available.");

    int capacity = singles * hotel->getSingle()->getCapacity()
                 + doubles * hotel->getDouble()->getCapacity();
    if (guest.getGuestCount() > capacity)
        throw invalid_argument("Selected rooms can hold only " + to_string(capacity) +
                               " guest(s), but you entered " +
                               to_string(guest.getGuestCount()) + ".");

    // reduce availability using the overloaded -- operator
    for (int i = 0; i < singles; i++) --(*hotel->getSingle());
    for (int i = 0; i < doubles; i++) --(*hotel->getDouble());

    total = calculateTotal();
    activeBookings++;
}

// Releases the rooms using the overloaded ++ operator
void Booking::cancel() {
    if (hotel == nullptr) return;
    for (int i = 0; i < singles; i++) ++(*hotel->getSingle());
    for (int i = 0; i < doubles; i++) ++(*hotel->getDouble());
}

string Booking::receiptText() const {
    stringstream out;
    out << fixed << setprecision(0);
    out << "  ---------------------------------------------------\n";
    out << "                  BOOKING RECEIPT\n";
    out << "  ---------------------------------------------------\n";
    out << "  Guest Name   : " << guest.getName() << "\n";
    out << "  CNIC         : " << guest.getCnic() << "\n";
    out << "  Phone        : " << guest.getPhone() << "\n";
    out << "  No. of Guests: " << guest.getGuestCount() << "\n";
    out << "  Hotel        : " << hotel->getName() << ", " << hotel->getCity() << "\n";
    out << "  Check-in     : " << checkIn << "   (" << nights << " night(s))\n";
    out << "  ---------------------------------------------------\n";

    if (singles > 0) {
        out << "  Single x " << singles << " @ PKR " << hotel->getSingle()->getPrice()
            << " x " << nights << " night(s) = PKR "
            << hotel->getSingle()->calculateCost(singles, nights) << "\n";
    }
    if (doubles > 0) {
        out << "  Double x " << doubles << " @ PKR " << hotel->getDouble()->getPrice()
            << " x " << nights << " night(s) = PKR "
            << hotel->getDouble()->calculateCost(doubles, nights);
        if (nights >= 5) out << "  (10% discount)";
        out << "\n";
    }
    if (scenery) {
        out << "  Scenic view  @ PKR " << hotel->getSceneryPrice() << " x " << nights
            << " night(s) = PKR " << hotel->getSceneryPrice() * nights << "\n";
        // overloaded + operator: room price + scenic charge per night
        Room* r = (doubles > 0) ? hotel->getDouble() : hotel->getSingle();
        out << "  (" << r->getType() << " room with view per night = PKR "
            << (*r + hotel->getSceneryPrice()) << ")\n";
    }
    out << "  ---------------------------------------------------\n";
    out << "  TOTAL AMOUNT : PKR " << total << "\n";
    out << "  ---------------------------------------------------\n";
    return out.str();
}

void Booking::printReceipt() const {
    cout << receiptText();
}

bool Booking::matches(const string& cnic) const {
    return guest == cnic;     // overloaded == of Guest
}

const Guest& Booking::getGuest() const { return guest; }
Hotel* Booking::getHotel() const { return hotel; }
double Booking::getTotal() const { return total; }
int Booking::getSingles() const { return singles; }
int Booking::getDoubles() const { return doubles; }
int Booking::getNights() const { return nights; }
string Booking::getCheckIn() const { return checkIn; }
bool Booking::hasScenery() const { return scenery; }

// Format: name|cnic|phone|guests|hotelIndex|singles|doubles|nights|checkIn|scenery|total
string Booking::toFileString() const {
    stringstream ss;
    ss << guest.getName() << "|" << guest.getCnic() << "|" << guest.getPhone() << "|"
       << guest.getGuestCount() << "|" << hotelIndex << "|" << singles << "|"
       << doubles << "|" << nights << "|" << checkIn << "|" << scenery << "|" << total;
    return ss.str();
}

// Builds a Booking from one line of guest_info.txt.
// Rooms are NOT reduced again because hotels.txt already stores availability.
bool Booking::fromFileString(const string& line, Hotel hotels[],
                             int hotelCount, Booking& out) {
    stringstream ss(line);
    string parts[11];
    for (int i = 0; i < 11; i++) {
        if (!getline(ss, parts[i], '|')) return false;
    }
    try {
        int idx = stoi(parts[4]);
        if (idx < 0 || idx >= hotelCount) return false;

        Guest g(parts[0], parts[1], parts[2], stoi(parts[3]));
        out = Booking(g, &hotels[idx], idx, stoi(parts[5]), stoi(parts[6]),
                      stoi(parts[7]), parts[8], parts[9] == "1");
        out.total = stod(parts[10]);
        activeBookings++;
        return true;
    } catch (const exception&) {
        return false;   // skip a damaged line instead of crashing
    }
}

int Booking::getActiveBookings() { return activeBookings; }

void Booking::decreaseActiveBookings() {
    if (activeBookings > 0) activeBookings--;
}

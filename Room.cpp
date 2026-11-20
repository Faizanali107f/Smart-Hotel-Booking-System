// ============================================================
//  Room.cpp  -  implementation of Room, SingleRoom, DoubleRoom
// ============================================================
#include "Room.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;

// ========================= Room =============================
Room::Room() : available(0), pricePerNight(0) {}

Room::Room(int available, double pricePerNight)
    : available(available), pricePerNight(pricePerNight) {}

Room::~Room() {}

int Room::getAvailable() const { return available; }
double Room::getPrice() const { return pricePerNight; }

void Room::setAvailable(int a) {
    if (a < 0) throw invalid_argument("Available rooms cannot be negative.");
    available = a;
}

void Room::setPrice(double p) {
    if (p <= 0) throw invalid_argument("Price must be greater than zero.");
    pricePerNight = p;
}

// Prefix ++ : a room is released (booking cancelled)
Room& Room::operator++() {
    available++;
    return *this;
}

// Prefix -- : a room is booked
Room& Room::operator--() {
    if (available <= 0)
        throw runtime_error("No " + getType() + " rooms are available.");
    available--;
    return *this;
}

// Binary + : nightly price plus an extra charge
double Room::operator+(double extra) const {
    return pricePerNight + extra;
}

// ====================== SingleRoom ==========================
SingleRoom::SingleRoom() : Room() {}

SingleRoom::SingleRoom(int available, double pricePerNight)
    : Room(available, pricePerNight) {}

double SingleRoom::calculateCost(int count, int nights) const {
    return count * nights * pricePerNight;
}

void SingleRoom::displayDetails() const {
    cout << setw(8) << available << " Single @ PKR "
         << fixed << setprecision(0) << setw(6) << pricePerNight;
}

string SingleRoom::getType() const { return "Single"; }
int SingleRoom::getCapacity() const { return 1; }
Room* SingleRoom::clone() const { return new SingleRoom(*this); }

// ====================== DoubleRoom ==========================
DoubleRoom::DoubleRoom() : Room() {}

DoubleRoom::DoubleRoom(int available, double pricePerNight)
    : Room(available, pricePerNight) {}

double DoubleRoom::calculateCost(int count, int nights) const {
    double cost = count * nights * pricePerNight;
    if (nights >= 5)
        cost = cost * 0.90;   // 10% family discount for long stays
    return cost;
}

void DoubleRoom::displayDetails() const {
    cout << setw(8) << available << " Double @ PKR "
         << fixed << setprecision(0) << setw(6) << pricePerNight;
}

string DoubleRoom::getType() const { return "Double"; }
int DoubleRoom::getCapacity() const { return 2; }
Room* DoubleRoom::clone() const { return new DoubleRoom(*this); }

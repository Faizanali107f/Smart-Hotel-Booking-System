// ============================================================
//  Hotel.cpp  -  implementation of Hotel
// ============================================================
#include "Hotel.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cctype>
using namespace std;

// helper: convert text to lower case for case-insensitive search
static string toLower(string s) {
    for (size_t i = 0; i < s.length(); i++)
        s[i] = tolower(s[i]);
    return s;
}

Hotel::Hotel() : name(""), city(""), sceneryPrice(0) {
    rooms[0] = new SingleRoom();
    rooms[1] = new DoubleRoom();
}

Hotel::Hotel(const string& name, const string& city,
             int singleAvail, double singlePrice,
             int doubleAvail, double doublePrice, double sceneryPrice)
    : name(name), city(city), sceneryPrice(sceneryPrice) {
    rooms[0] = new SingleRoom(singleAvail, singlePrice);
    rooms[1] = new DoubleRoom(doubleAvail, doublePrice);
}

// Deep copy: each hotel gets its own Room objects
Hotel::Hotel(const Hotel& other)
    : name(other.name), city(other.city), sceneryPrice(other.sceneryPrice) {
    rooms[0] = other.rooms[0]->clone();
    rooms[1] = other.rooms[1]->clone();
}

Hotel& Hotel::operator=(const Hotel& other) {
    if (this != &other) {
        delete rooms[0];
        delete rooms[1];
        name = other.name;
        city = other.city;
        sceneryPrice = other.sceneryPrice;
        rooms[0] = other.rooms[0]->clone();
        rooms[1] = other.rooms[1]->clone();
    }
    return *this;
}

// Composition: when the hotel is destroyed, its rooms are destroyed too
Hotel::~Hotel() {
    delete rooms[0];
    delete rooms[1];
}

void Hotel::setData(const string& n, const string& c,
                    int singleAvail, double singlePrice,
                    int doubleAvail, double doublePrice, double scenery) {
    name = n;
    city = c;
    sceneryPrice = scenery;
    rooms[0]->setAvailable(singleAvail);
    rooms[0]->setPrice(singlePrice);
    rooms[1]->setAvailable(doubleAvail);
    rooms[1]->setPrice(doublePrice);
}

string Hotel::getName() const { return name; }
string Hotel::getCity() const { return city; }
double Hotel::getSceneryPrice() const { return sceneryPrice; }

void Hotel::setSceneryPrice(double p) {
    if (p < 0) throw invalid_argument("Scenery price cannot be negative.");
    sceneryPrice = p;
}

Room* Hotel::getSingle() const { return rooms[0]; }
Room* Hotel::getDouble() const { return rooms[1]; }

bool Hotel::matches(const string& key) const {
    return toLower(city) == toLower(key);
}

void Hotel::displayDetails(int number) const {
    cout << "  " << setw(2) << number << ". " << left << setw(30) << name
         << right << " |";
    rooms[0]->displayDetails();      // polymorphic call
    cout << " |";
    rooms[1]->displayDetails();      // polymorphic call
    cout << " | View +" << fixed << setprecision(0) << sceneryPrice << endl;
}

// Format: name|city|singleAvail|singlePrice|doubleAvail|doublePrice|scenery
string Hotel::toFileString() const {
    stringstream ss;
    ss << name << "|" << city << "|"
       << rooms[0]->getAvailable() << "|" << rooms[0]->getPrice() << "|"
       << rooms[1]->getAvailable() << "|" << rooms[1]->getPrice() << "|"
       << sceneryPrice;
    return ss.str();
}

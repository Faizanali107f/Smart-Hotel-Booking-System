// ============================================================
//  Person.cpp  -  implementation of Person, Guest and Admin
// ============================================================
#include "Person.h"
#include <iostream>
using namespace std;

// ======================== Person ============================
Person::Person() : name("Unknown") {}

Person::Person(const string& name) : name(name) {}

Person::~Person() {}

string Person::getName() const { return name; }

void Person::setName(const string& n) { name = n; }

// ======================== Guest =============================
Guest::Guest() : Person(), cnic(""), phone(""), guestCount(0) {}

Guest::Guest(const string& name, const string& cnic,
             const string& phone, int guestCount)
    : Person(name), cnic(cnic), phone(phone), guestCount(guestCount) {}

// Copy constructor: copies every field of another guest
Guest::Guest(const Guest& other)
    : Person(other.name), cnic(other.cnic),
      phone(other.phone), guestCount(other.guestCount) {}

Guest& Guest::operator=(const Guest& other) {
    if (this != &other) {
        name = other.name;
        cnic = other.cnic;
        phone = other.phone;
        guestCount = other.guestCount;
    }
    return *this;
}

Guest::~Guest() {}

string Guest::getCnic() const { return cnic; }
string Guest::getPhone() const { return phone; }
int Guest::getGuestCount() const { return guestCount; }

void Guest::displayDetails() const {
    cout << "  Guest Name   : " << name << endl;
    cout << "  CNIC         : " << cnic << endl;
    cout << "  Phone        : " << phone << endl;
    cout << "  No. of Guests: " << guestCount << endl;
}

string Guest::getRole() const { return "Guest"; }

bool Guest::operator==(const Guest& other) const {
    return cnic == other.cnic;
}

bool Guest::operator==(const string& otherCnic) const {
    return cnic == otherCnic;
}

// ======================== Admin =============================
Admin::Admin() : Person("Administrator"), username("admin"), password("admin123") {}

Admin::Admin(const string& name, const string& username, const string& password)
    : Person(name), username(username), password(password) {}

Admin::~Admin() {}

bool Admin::login(const string& user, const string& pass) const {
    return user == username && pass == password;
}

void Admin::displayDetails() const {
    cout << "  Logged in as : " << name << " (" << username << ")" << endl;
    cout << "  Role         : " << getRole() << endl;
}

string Admin::getRole() const { return "Hotel Administrator"; }

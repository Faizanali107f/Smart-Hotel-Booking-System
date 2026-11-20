// ============================================================
//  Booking.h
//  Class: Booking
//
//  Concepts: AGGREGATION - a Booking refers to an existing Hotel
//            (through a pointer) and stores the Guest's details.
//            Static member (count of active bookings),
//            const member functions, exception handling,
//            file handling (toFileString / fromFileString).
// ============================================================
#ifndef BOOKING_H
#define BOOKING_H

#include <string>
#include "Person.h"
#include "Hotel.h"

class Booking {
private:
    Guest guest;
    Hotel* hotel;          // aggregation: hotel exists independently
    int hotelIndex;        // position of hotel in the hotels array
    int singles;
    int doubles;
    int nights;
    std::string checkIn;   // DD-MM-YYYY
    bool scenery;
    double total;

    // prices are copied from the hotel when the booking is made, so a
    // later tariff change by the admin does not change old receipts
    double singlePrice;
    double doublePrice;
    double sceneryPrice;

    static int activeBookings;   // shared by all Booking objects

public:
    Booking();
    Booking(const Guest& guest, Hotel* hotel, int hotelIndex,
            int singles, int doubles, int nights,
            const std::string& checkIn, bool scenery);

    double calculateTotal() const;
    void confirm();         // books the rooms (may throw)
    void cancel();          // releases the rooms

    std::string receiptText() const;   // receipt as text (used by console and GUI)
    void printReceipt() const;

    // used by SearchUtility<Booking> : true if CNIC matches
    bool matches(const std::string& cnic) const;

    const Guest& getGuest() const;
    Hotel* getHotel() const;
    double getTotal() const;
    int getSingles() const;
    int getDoubles() const;
    int getNights() const;
    std::string getCheckIn() const;
    bool hasScenery() const;

    std::string toFileString() const;
    static bool fromFileString(const std::string& line, Hotel hotels[],
                               int hotelCount, Booking& out);

    static int getActiveBookings();
    static void decreaseActiveBookings();
};

#endif

// ============================================================
//  HotelManager.h
//  Class: HotelManager
//
//  Keeps all hotels and bookings in memory and saves them to
//  hotels.txt and guest_info.txt. Both the console version
//  (main.cpp) and the Qt GUI (gui/) use this class, so the
//  booking logic is written only once.
//
//  Concepts: arrays of objects (dynamic), composition (the manager
//            owns the arrays), static constants, templates
//            (SearchUtility<T>), exception handling, file handling.
// ============================================================
#ifndef HOTELMANAGER_H
#define HOTELMANAGER_H

#include <string>
#include "Hotel.h"
#include "Booking.h"

class HotelManager {
public:
    static const int MAX_HOTELS = 40;
    static const int MAX_BOOKINGS = 500;
    static const int CITY_COUNT = 8;
    static const std::string CITIES[CITY_COUNT];
    static const std::string HOTEL_FILE;
    static const std::string BOOKING_FILE;

private:
    Hotel* hotels;          // array of 40 Hotel objects
    Booking* bookings;      // array of Booking objects
    int hotelCount;
    int bookingCount;
    bool createdDefaultData;

    void createDefaultHotelFile();

public:
    HotelManager();
    ~HotelManager();

    // a manager owns its arrays, so copying is not allowed
    HotelManager(const HotelManager&) = delete;
    HotelManager& operator=(const HotelManager&) = delete;

    void load();            // reads both files (throws runtime_error)
    void save() const;      // writes both files (throws runtime_error)
    bool isFirstRun() const;

    int getHotelCount() const;
    int getBookingCount() const;
    Hotel& getHotel(int index);
    const Hotel& getHotel(int index) const;
    const Booking& getBooking(int index) const;

    // searches (use SearchUtility<T>)
    int findHotelsByCity(const std::string& city, int results[], int maxResults) const;
    int findBookingsByCnic(const std::string& cnic, int results[], int maxResults) const;

    // creates a booking for hotels[hotelIndex]; throws if not possible
    void addBooking(const Guest& guest, int hotelIndex, int singles, int doubles,
                    int nights, const std::string& checkIn, bool scenery);
    void cancelBooking(int bookingIndex);

    // price of a booking before it is confirmed
    double estimateTotal(int hotelIndex, int singles, int doubles,
                         int nights, bool scenery) const;

    // reports for the admin
    double revenueForCity(const std::string& city) const;
    double totalRevenue() const;
    int freeSingleRooms() const;
    int freeDoubleRooms() const;
};

#endif

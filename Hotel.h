// ============================================================
//  Hotel.h
//  Class: Hotel
//
//  Concepts: COMPOSITION - a Hotel owns its two Room objects
//            (created in the constructor, deleted in the destructor).
//            Room* pointers are used because Room is abstract.
//            Deep copy via copy constructor + assignment operator.
// ============================================================
#ifndef HOTEL_H
#define HOTEL_H

#include <string>
#include "Room.h"

class Hotel {
private:
    std::string name;
    std::string city;
    double sceneryPrice;       // per-night charge for a scenic view
    Room* rooms[2];            // rooms[0] = SingleRoom, rooms[1] = DoubleRoom

public:
    Hotel();
    Hotel(const std::string& name, const std::string& city,
          int singleAvail, double singlePrice,
          int doubleAvail, double doublePrice, double sceneryPrice);
    Hotel(const Hotel& other);               // deep copy
    Hotel& operator=(const Hotel& other);    // deep copy assignment
    ~Hotel();

    void setData(const std::string& name, const std::string& city,
                 int singleAvail, double singlePrice,
                 int doubleAvail, double doublePrice, double sceneryPrice);

    std::string getName() const;
    std::string getCity() const;
    double getSceneryPrice() const;
    void setSceneryPrice(double p);

    Room* getSingle() const;
    Room* getDouble() const;

    // used by SearchUtility<Hotel> : true if city matches (case-insensitive)
    bool matches(const std::string& key) const;

    void displayDetails(int number) const;
    std::string toFileString() const;        // one line for hotels.txt
};

#endif

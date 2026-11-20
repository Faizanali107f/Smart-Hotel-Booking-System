// ============================================================
//  Room.h
//  Classes: Room (abstract base), SingleRoom, DoubleRoom
//
//  NOTE: One Room object represents a ROOM TYPE inside a hotel
//  (e.g. "Double rooms of Avari Lahore") together with how many
//  of those rooms are still available and the price per night.
//
//  Concepts: inheritance, pure virtual functions, polymorphism,
//            unary operator overloading (++ / --),
//            binary operator overloading (+)
// ============================================================
#ifndef ROOM_H
#define ROOM_H

#include <string>

class Room {
protected:
    int available;          // rooms of this type still free
    double pricePerNight;   // tariff in PKR

public:
    Room();
    Room(int available, double pricePerNight);
    virtual ~Room();

    int getAvailable() const;
    double getPrice() const;
    void setAvailable(int a);
    void setPrice(double p);

    // ++room : one room becomes free again (cancellation)
    Room& operator++();
    // --room : one room gets booked (throws if none are left)
    Room& operator--();
    // room + extra : nightly price plus an extra charge (e.g. scenic view)
    double operator+(double extra) const;

    // Pure virtual functions -> Room is abstract
    virtual double calculateCost(int count, int nights) const = 0;
    virtual void displayDetails() const = 0;
    virtual std::string getType() const = 0;
    virtual int getCapacity() const = 0;   // persons per room
    virtual Room* clone() const = 0;       // used for deep copies
};

// ------------------------------------------------------------
class SingleRoom : public Room {
public:
    SingleRoom();
    SingleRoom(int available, double pricePerNight);

    double calculateCost(int count, int nights) const override;
    void displayDetails() const override;
    std::string getType() const override;
    int getCapacity() const override;
    Room* clone() const override;
};

// ------------------------------------------------------------
// DoubleRoom gives a 10% family discount on stays of 5+ nights
// ------------------------------------------------------------
class DoubleRoom : public Room {
public:
    DoubleRoom();
    DoubleRoom(int available, double pricePerNight);

    double calculateCost(int count, int nights) const override;
    void displayDetails() const override;
    std::string getType() const override;
    int getCapacity() const override;
    Room* clone() const override;
};

#endif

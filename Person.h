// ============================================================
//  Person.h
//  Smart Hotel Booking & Management System
//
//  Classes: Person (abstract base), Guest, Admin
//  Concepts: inheritance (hierarchical), pure virtual functions,
//            constructors (default / parameterized / copy),
//            destructors, operator overloading (==)
// ============================================================
#ifndef PERSON_H
#define PERSON_H

#include <string>

// ------------------------------------------------------------
// Person : abstract base class (cannot create objects of it)
// ------------------------------------------------------------
class Person {
protected:
    std::string name;

public:
    Person();                              // default constructor
    Person(const std::string& name);       // parameterized constructor
    virtual ~Person();                     // virtual destructor

    std::string getName() const;
    void setName(const std::string& n);

    // Pure virtual functions -> makes Person abstract
    virtual void displayDetails() const = 0;
    virtual std::string getRole() const = 0;
};

// ------------------------------------------------------------
// Guest : a person who books rooms (identified by CNIC)
// ------------------------------------------------------------
class Guest : public Person {
private:
    std::string cnic;
    std::string phone;
    int guestCount;

public:
    Guest();
    Guest(const std::string& name, const std::string& cnic,
          const std::string& phone, int guestCount);
    Guest(const Guest& other);             // copy constructor
    Guest& operator=(const Guest& other);  // copy assignment
    ~Guest();

    std::string getCnic() const;
    std::string getPhone() const;
    int getGuestCount() const;

    void displayDetails() const override;
    std::string getRole() const override;

    // Operator overloading: compare two guests / guest with a CNIC
    bool operator==(const Guest& other) const;
    bool operator==(const std::string& otherCnic) const;
};

// ------------------------------------------------------------
// Admin : hotel administrator with password-protected login
// ------------------------------------------------------------
class Admin : public Person {
private:
    std::string username;
    std::string password;

public:
    Admin();
    Admin(const std::string& name, const std::string& username,
          const std::string& password);
    ~Admin();

    bool login(const std::string& user, const std::string& pass) const;

    void displayDetails() const override;
    std::string getRole() const override;
};

#endif

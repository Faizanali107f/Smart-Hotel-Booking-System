// ============================================================
//  SearchUtility.h   (header-only template)
//  Class: SearchUtility<T>
//
//  Concepts: TEMPLATES - the same search code works for any class
//            that has a  bool matches(const std::string&) const
//            function. Used for:
//              SearchUtility<Hotel>   -> search hotels by city
//              SearchUtility<Booking> -> search bookings by CNIC
// ============================================================
#ifndef SEARCHUTILITY_H
#define SEARCHUTILITY_H

#include <string>

template <typename T>
class SearchUtility {
public:
    // Returns index of the first match, or -1 if nothing matches
    static int findFirst(const T arr[], int size, const std::string& key) {
        for (int i = 0; i < size; i++) {
            if (arr[i].matches(key))
                return i;
        }
        return -1;
    }

    // Stores indexes of all matches in results[], returns how many found
    static int findAll(const T arr[], int size, const std::string& key,
                       int results[], int maxResults) {
        int found = 0;
        for (int i = 0; i < size && found < maxResults; i++) {
            if (arr[i].matches(key))
                results[found++] = i;
        }
        return found;
    }
};

#endif

// ============================================================
//  InputValidator.cpp  -  implementation of InputValidator
// ============================================================
#include "InputValidator.h"
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
using namespace std;

// ------------------------- validators -------------------------
void InputValidator::validateName(const string& s) {
    if (s.length() < 3)
        throw invalid_argument("Name must be at least 3 characters long.");
    for (size_t i = 0; i < s.length(); i++) {
        if (!isalpha(s[i]) && s[i] != ' ' && s[i] != '.')
            throw invalid_argument("Name can contain only letters, spaces and dots.");
    }
}

// Checks text against a pattern where '0' means "any digit"
static bool matchesPattern(const string& s, const string& pattern) {
    if (s.length() != pattern.length()) return false;
    for (size_t i = 0; i < s.length(); i++) {
        if (pattern[i] == '0') {
            if (!isdigit(s[i])) return false;
        } else if (s[i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

void InputValidator::validateCNIC(const string& s) {
    if (!matchesPattern(s, "00000-0000000-0"))
        throw invalid_argument("CNIC must be in the format 00000-0000000-0.");
}

void InputValidator::validatePhone(const string& s) {
    if (!matchesPattern(s, "0000-0000000"))
        throw invalid_argument("Phone must be in the format 0000-0000000.");
    if (s.substr(0, 2) != "03")
        throw invalid_argument("Mobile number must start with 03.");
}

void InputValidator::validateDate(const string& s) {
    if (!matchesPattern(s, "00-00-0000"))
        throw invalid_argument("Date must be in the format DD-MM-YYYY.");

    int day = stoi(s.substr(0, 2));
    int month = stoi(s.substr(3, 2));
    int year = stoi(s.substr(6, 4));

    if (year < 2025 || year > 2030)
        throw invalid_argument("Year must be between 2025 and 2030.");
    if (month < 1 || month > 12)
        throw invalid_argument("Month must be between 01 and 12.");

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (leap) daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1])
        throw invalid_argument("This month does not have day " + to_string(day) + ".");
}

int InputValidator::toInt(const string& s, int min, int max) {
    if (s.empty())
        throw invalid_argument("Please enter a number.");
    for (size_t i = 0; i < s.length(); i++) {
        if (!isdigit(s[i]))
            throw invalid_argument("Please enter a whole number only.");
    }
    if (s.length() > 6)
        throw invalid_argument("Number is too large.");
    int value = stoi(s);
    if (value < min || value > max)
        throw invalid_argument("Please enter a number between " + to_string(min) +
                               " and " + to_string(max) + ".");
    return value;
}

double InputValidator::toDouble(const string& s, double min, double max) {
    size_t used = 0;
    double value;
    try {
        value = stod(s, &used);
    } catch (const exception&) {
        throw invalid_argument("Please enter a valid amount.");
    }
    if (used != s.length())
        throw invalid_argument("Please enter a valid amount.");
    if (value < min || value > max)
        throw invalid_argument("Amount must be between " + to_string((int)min) +
                               " and " + to_string((int)max) + ".");
    return value;
}

// ------------------------- readers ----------------------------
string InputValidator::readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        // input stream closed (e.g. Ctrl+Z / end of file) -> exit cleanly
        cout << "\n  Input ended. Goodbye!\n";
        exit(0);
    }
    // trim spaces at both ends
    size_t start = line.find_first_not_of(" \t\r");
    size_t end = line.find_last_not_of(" \t\r");
    if (start == string::npos) return "";
    return line.substr(start, end - start + 1);
}

string InputValidator::readValid(const string& prompt,
                                 void (*validator)(const string&)) {
    while (true) {
        string input = readLine(prompt);
        try {
            validator(input);
            return input;
        } catch (const invalid_argument& e) {
            cout << "  [!] " << e.what() << " Try again.\n";
        }
    }
}

int InputValidator::readInt(const string& prompt, int min, int max) {
    while (true) {
        string input = readLine(prompt);
        try {
            return toInt(input, min, max);
        } catch (const invalid_argument& e) {
            cout << "  [!] " << e.what() << " Try again.\n";
        }
    }
}

double InputValidator::readDouble(const string& prompt, double min, double max) {
    while (true) {
        string input = readLine(prompt);
        try {
            return toDouble(input, min, max);
        } catch (const invalid_argument& e) {
            cout << "  [!] " << e.what() << " Try again.\n";
        }
    }
}

bool InputValidator::readYesNo(const string& prompt) {
    while (true) {
        string input = readLine(prompt);
        if (input == "y" || input == "Y") return true;
        if (input == "n" || input == "N") return false;
        cout << "  [!] Please enter y or n.\n";
    }
}

// ============================================================
//  InputValidator.h
//  Class: InputValidator
//
//  Concepts: EXCEPTION HANDLING - every validate function throws
//            std::invalid_argument when the input is wrong. The
//            read functions catch the exception and ask again
//            (this replaces the old goto loops).
// ============================================================
#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <string>

class InputValidator {
public:
    // validators (throw std::invalid_argument on bad input)
    static void validateName(const std::string& s);
    static void validateCNIC(const std::string& s);    // 00000-0000000-0
    static void validatePhone(const std::string& s);   // 0000-0000000
    static void validateDate(const std::string& s);    // DD-MM-YYYY
    static int  toInt(const std::string& s, int min, int max);
    static double toDouble(const std::string& s, double min, double max);

    // read helpers (keep asking until the input is valid)
    static std::string readLine(const std::string& prompt);
    static std::string readValid(const std::string& prompt,
                                 void (*validator)(const std::string&));
    static int readInt(const std::string& prompt, int min, int max);
    static double readDouble(const std::string& prompt, double min, double max);
    static bool readYesNo(const std::string& prompt);
};

#endif

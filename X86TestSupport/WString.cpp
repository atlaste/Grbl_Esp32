#include "WString.h"

#include <iomanip>
#include <sstream>

#pragma warning(disable : 4996) // itoa

std::string String::ValueToString(int value, int base) {
    char        buffer[100] = { 0 };
    int         number_base = 10;
    std::string output = itoa(value, buffer, base);
    return output;
}

std::string String::DecToString(double value, int decimalPlaces) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(decimalPlaces) << value;
    std::string s = stream.str();
    return s;
}

StringAppender& operator +(const StringAppender& lhs, const String& rhs) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(rhs);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, const char* cstr) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(cstr);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, char c) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(c);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, unsigned char num) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(num);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, int num) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(num);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, unsigned int num) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(num);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, long num) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(num);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, unsigned long num) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(num);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, float num) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(num);
    return a;
}

StringAppender& operator +(const StringAppender& lhs, double num) {
    StringAppender& a = const_cast<StringAppender&>(lhs);
    a.concat(num);
    return a;
}

#include "AssertionFailed.h"

#include <cstdarg>
#include <cstring>

#ifdef ESP32
#    ifdef UNIT_TEST

#        include "debug_helpers.h"
#        include "WString.h"

AssertionFailed AssertionFailed::create(const char* condition, const char* msg, ...) {
    String st = condition;
    st += ": ";

    char    tmp[255];
    va_list arg;
    va_start(arg, msg);
    size_t  len = vsnprintf(tmp, 255, msg, arg);
    tmp[254] = 0;
    st += tmp;

    st += " at: ";
    st += esp_backtrace_print(10);

    return AssertionFailed(st);
}

#    else

AssertionFailed AssertionFailed::create(const char* condition, const char* msg, ...) {
    String st = "\r\nError ";
    st += condition;
    st += " failed: ";

    char    tmp[255];
    va_list arg;    
    va_start(arg, msg);
    size_t  len = vsnprintf(tmp, 255, msg, arg);
    tmp[254]    = 0;
    st += tmp;

    return AssertionFailed(st);
}

#    endif

#else

#    include <iostream>
#    include <string>
#    include <sstream>
#    include "WString.h"

extern void DumpStackTrace(std::ostringstream& builder);

String stackTrace;

std::exception AssertionFailed::create(const char* condition, const char* msg, ...) {
    std::ostringstream oss;
    oss << std::endl;
    oss << "Error: " << std::endl;

    char    tmp[255];
    va_list arg;
    va_start(arg, msg);
    size_t  len = vsnprintf(tmp, 255, msg, arg);
    tmp[254]    = 0;
    oss << tmp;

    oss << " at ";
    DumpStackTrace(oss);

    // Store in a static temp:
    static std::string info;
    info = oss.str();
    throw std::exception(info.c_str());
}


#endif

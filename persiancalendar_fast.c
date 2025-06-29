// This code is based on the Python code published by Roozbeh
// Pournader under the Apache 2.0 license which is in turn 
// partially based on the Common Lisp code published by
// Reingold and Dershowitz under the Apache 2.0 license.

// C port was made by Navid Rashidian.

// Copyright 2025 Navid Rashidian

// The original header follows:

// This code is partially based on the Common Lisp code published by
// Reingold and Dershowitz under the Apache 2.0 license.

// Python port and modifications for using the 33-year cycle were
// made by Roozbeh Pournader.

// Copyright 2024 Roozbeh Pournader

// CALENDRICA 4.0 -- Common Lisp
// E. M. Reingold and N. Dershowitz

// ================================================================

// The Functions (code, comments, and definitions) contained in this
// file (the "Program") were written by Edward M. Reingold and Nachum
// Dershowitz (the "Authors"), who retain all rights to them except as
// granted in the License and subject to the warranty and liability
// limitations listed therein.  These Functions are explained in the Authors'
// book, "Calendrical Calculations", 4th ed. (Cambridge University
// Press, 2016), and are subject to an international copyright.

// Licensed under the Apache License, Version 2.0 <LICENSE or
// https://www.apache.org/licenses/LICENSE-2.0>.

// Sample values for the functions (useful for debugging) are given in
// Appendix C of the book.

#include "persiancalendar_fast.h"

#include <stdbool.h>

#define GREGORIAN_EPOCH 1
#define PERSIAN_EPOCH 226896  // Precalculated result from Calendrical Calculations

const int SUPPORTED_FIRST_YEAR = 1178;
const int SUPPORTED_LAST_YEAR  = 3000;

// Precalculated bit-packed array of non-leap years

static const unsigned char NON_LEAP_LOOKUP[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 
    0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
    0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 
    0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 
    0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 
    0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 
    0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x22, 0x00, 
    0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00, 
    0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x20, 0x02, 
    0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x80, 0x08, 
    0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x22, 
    0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x88, 
    0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x20, 
    0x02, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x80, 
    0x08, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 
    0x22, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 
    0x88, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 
    0x20, 0x02, 0x00, 0x00, 0x44, 0x04, 0x00, 0x00, 
    0x88, 0x08, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 
    0x20, 0x22, 0x00, 0x00, 0x40, 0x44, 0x00, 0x00, 
    0x80, 0x88, 0x00, 0x00, 0x00, 0x11, 0x01, 0x00, 
    0x00, 0x22, 0x02, 0x00, 
};

bool is_non_leap_correction(int year) {
    if (year < SUPPORTED_FIRST_YEAR || year > SUPPORTED_LAST_YEAR) return false;
    int adjusted_year = year - SUPPORTED_FIRST_YEAR;
    return (NON_LEAP_LOOKUP[adjusted_year / 8] & (1 << (adjusted_year % 8))) != 0;
}

bool gregorian_leap_year(int g_year) {
    // True if g_year is a leap year on the Gregorian calendar.
    int r = g_year % 400;
    return (g_year % 4 == 0) && (r != 100 && r != 200 && r != 300);
}

int fixed_from_gregorian(GregorianDate g_date) {
    int year = g_date.year;
    int month = g_date.month;
    int day = g_date.day;
    // Fixed date equivalent to the Gregorian date g_date.
    return (
        GREGORIAN_EPOCH - 1  /* Days before start of calendar */
        + 365 * (year - 1)  /* Ordinary days since epoch */
        + (year - 1) / 4   /* Julian leap days since epoch... */
        - (year - 1) / 100  /* ...minus century years since epoch... */
        + (year - 1) / 400  /* plus years since epoch divisible by 400. */
        /* Days in prior months this year assuming 30-day Feb */
        + (367 * month - 362) / 12
        /*  Correct for 28- or 29-day Feb */
        + (month <= 2 ? 0 : (gregorian_leap_year(year) ? -1 : -2))
        + day);  // Days so far this month.
}

int fixed_from_persian_fast(PersianDate p_date) {
    int new_year = PERSIAN_EPOCH - 1 + 365 * (p_date.year - 1) + (8 * p_date.year + 21) / 33;
    if (is_non_leap_correction(p_date.year - 1)) new_year -= 1;
    return (new_year - 1) /* Days in prior years. */
        /* Days in prior months this year. */
       + (p_date.month <= 7 ? 31 * (p_date.month - 1) : 30 * (p_date.month - 1) + 6)
       + p_date.day; // Days so far this month.
}

int div_ceil(int a, int b) {
    // Equivalent to Python math.ceil(a / b), but with no floating point math
    return (a + b - 1) / b;
}


PersianDate persian_fast_from_fixed(int date) {
    PersianDate epoch = {1, 1, 1};
    int days_since_epoch = date - fixed_from_persian_fast(epoch);
    int year = 1 + (33 * days_since_epoch + 3) / 12053;
    PersianDate start_of_year = {year, 1, 1};
    int day_of_year = date - fixed_from_persian_fast(start_of_year) + 1;
    if (day_of_year == 366 && is_non_leap_correction(year)) {
        year += 1;
        day_of_year = 1;
    }
    int month;
    if (day_of_year <= 186) { month = div_ceil(day_of_year, 31); }
    else { month = div_ceil(day_of_year - 6, 30); };
    // Calculate the day by subtraction
    PersianDate pd = {year, month, 1};
    pd.day = date - fixed_from_persian_fast(pd) + 1;
    return pd;
}

int persian_fast_leap_year(int p_year) {
    if (is_non_leap_correction(p_year)) return false;
    else if (is_non_leap_correction(p_year - 1)) return true;
    else return (25 * p_year + 11) % 33 < 8;
}

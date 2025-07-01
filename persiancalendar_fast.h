#ifndef PERSIAN_CALENDAR
#define PERSIAN_CALENDAR

#ifdef __cplusplus
extern "C" {
#endif

extern const int SUPPORTED_FIRST_YEAR;
extern const int SUPPORTED_LAST_YEAR;

typedef struct {
    int year;
    int month;
    int day;
} PersianDate;

typedef struct {
    int year;
    int month;
    int day;
} GregorianDate;

int fixed_from_gregorian(GregorianDate g_date);
int fixed_from_persian_fast(PersianDate p_date);
PersianDate persian_fast_from_fixed(int date);

#ifdef __cplusplus
}
#endif

#endif
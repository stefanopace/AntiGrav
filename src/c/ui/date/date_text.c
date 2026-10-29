#include "date_text.h"

#include "../../settings.h"

static const char s_day_abbr[12][7][5] = {
    {"Do", "Lu", "Ma", "Me", "Gi", "Ve", "Sa"},
    {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"},
    {"Do", "Lu", "Ma", "Mi", "Ju", "Vi", "Sa"},
    {"Di", "Lu", "Ma", "Me", "Je", "Ve", "Sa"},
    {"Do", "Se", "Te", "Qa", "Qi", "Sx", "Sa"},
    {"Mi", "Sn", "Sl", "Ra", "Ka", "Ju", "Sa"},
    {"So*", "Mo*", "Di*", "Mi*", "Do*", "Fr*", "Sa*"},
    {"Zo*", "Ma*", "Di*", "Wo*", "Do*", "Vr*", "Za*"},
    {"Sö", "Må", "Ti", "On", "To", "Fr", "Lö"},
    {"Sø", "Ma", "Ti", "On", "To", "Fr", "Lø"},
    {"Li", "Lu", "Ma", "Mi", "Hu", "Bi", "Sa"},
    {"Sø", "Ma", "Ti", "On", "To", "Fr", "Lø"},
};

static int language_row(int language);

const char *date_weekday_abbr(int language, int wday)
{
    if (wday < 0 || wday >= 7)
    {
        return "";
    }
    return s_day_abbr[language_row(language)][wday];
}

char date_numeric_separator(void)
{
    switch (settings_get_date_separator())
    {
    case DATE_SEPARATOR_DOT:
        return '.';
    case DATE_SEPARATOR_HYPHEN:
        return '-';
    case DATE_SEPARATOR_SLASH:
    default:
        return '/';
    }
}

static int language_row(int language)
{
    if (language < 0 || language >= 12)
    {
        return 1;
    }
    return language;
}

#include "date_text.h"

#include "../../settings.h"

static const char s_day_abbr[12][7][5] = {
    {"DOM", "LUN", "MAR", "MER", "GIO", "VEN", "SAB"},
    {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"},
    {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"},
    {"DIM", "LUN", "MAR", "MER", "JEU", "VEN", "SAM"},
    {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"},
    {"MIN", "SEN", "SEL", "RAB", "KAM", "JUM", "SAB"},
    {"SO.", "MO.", "DI.", "MI.", "DO.", "FR.", "SA."},
    {"ZO.", "MA.", "DI.", "WO.", "DO.", "VR.", "ZA."},
    {"SÖN", "MÅN", "TIS", "ONS", "TOR", "FRE", "LÖR"},
    {"SØN", "MAN", "TIR", "ONS", "TOR", "FRE", "LØR"},
    {"LIN", "LUN", "MAR", "MIY", "HUW", "BIY", "SAB"},
    {"SØN", "MAN", "TIR", "ONS", "TOR", "FRE", "LØR"},
};

static const char s_month_abbr[12][12][4] = {
    {"GEN", "FEB", "MAR", "APR", "MAG", "GIU", "LUG", "AGO", "SET", "OTT", "NOV", "DIC"},
    {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"},
    {"ENE", "FEB", "MAR", "ABR", "MAY", "JUN", "JUL", "AGO", "SEP", "OCT", "NOV", "DIC"},
    {"JAN", "FEV", "MAR", "AVR", "MAI", "JUN", "JUL", "AOU", "SEP", "OCT", "NOV", "DEC"},
    {"JAN", "FEV", "MAR", "ABR", "MAI", "JUN", "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"},
    {"JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES"},
    {"JAN", "FEB", "MRZ", "APR", "MAI", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEZ"},
    {"JAN", "FEB", "MRT", "APR", "MEI", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC"},
    {"JAN", "FEB", "MAR", "APR", "MAJ", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC"},
    {"JAN", "FEB", "MAR", "APR", "MAI", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DES"},
    {"ENE", "PEB", "MAR", "ABR", "MAY", "HUN", "HUL", "AGO", "SET", "OKT", "NOB", "DIS"},
    {"JAN", "FEB", "MAR", "APR", "MAJ", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC"},
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

const char *date_month_abbr(int language, int month)
{
    if (month < 0 || month >= 12)
    {
        return "";
    }
    return s_month_abbr[language_row(language)][month];
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

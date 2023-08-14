#ifndef DATES_H
#define DATES_H
#include "dates.c"

void parse_date(char *date_string, Date *date_buffer);

int getDifference(Date dt1, Date dt2);

int getDifference_str(char *date_1, char *date_2);

#endif

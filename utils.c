#include <time.h>
#include <stdio.h>

void print_time()
{
    time_t raw_time;
    struct tm *time_info;
    char iso_time[50] = {'\0'};

    time(&raw_time);
    time_info = localtime(&raw_time);

    // Format the time as ISO 8601
    strftime(iso_time, sizeof(iso_time), "%Y-%m-%dT%H:%M:%S%z", time_info);
    printf("%s\n", iso_time);
}

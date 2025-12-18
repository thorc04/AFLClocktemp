#pragma once
#include <RTClib.h>

void rtc_begin();
DateTime rtc_now();
void rtc_set(int y, int mo, int d, int h, int mi, int s);

#include "rtc_service.h"
#include <Arduino.h>

static RTC_DS3231 rtc;

void rtc_begin() {
  if (!rtc.begin()) {
    Serial.begin(9600);
    Serial.println("Couldn't find DS3231");
    while (1) {}
  }

  // Kun hvis den har mistet strøm
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

DateTime rtc_now() {
  return rtc.now();
}

void rtc_set(int y, int mo, int d, int h, int mi, int s) {
  rtc.adjust(DateTime(y, mo, d, h, mi, s));
}

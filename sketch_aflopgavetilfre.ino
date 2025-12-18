#include <Wire.h>
#include "ui.h"
#include "rtc_service.h"
#include "dht_service.h"
#include "input.h"

void setup() {
  Wire.begin();
  rtc_begin();
  dht_begin();
  ui_begin();
  input_begin();

  ui_showSplash();
}

void loop() {
  input_update();
  ui_update();
}

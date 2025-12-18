#pragma once
#include <Arduino.h>

void dht_begin();
bool dht_read(float &tC, float &hum);

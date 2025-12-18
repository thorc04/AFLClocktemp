#include "dht_service.h"
#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT11

static DHT dht(DHTPIN, DHTTYPE);

void dht_begin() {
  dht.begin();
}

bool dht_read(float &tC, float &hum) {
  hum = dht.readHumidity();
  tC  = dht.readTemperature();
  if (isnan(hum) || isnan(tC)) return false;
  return true;
}

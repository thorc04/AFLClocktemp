#include "ui.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "rtc_service.h"
#include "dht_service.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static ScreenId current = SCREEN_CLOCK;
static bool editMode = false;

// non-blocking refresh
static unsigned long lastDrawMs = 0;

// EDIT state
enum EditField { EF_YEAR, EF_MONTH, EF_DAY, EF_HOUR, EF_MINUTE, EF_COUNT };
static EditField field = EF_YEAR;
static int eY, eMo, eD, eH, eMi;

// -------- helpers
static void twoDigits(int v) {
  if (v < 10) display.print('0');
  display.print(v);
}

static int daysInMonth(int y, int m) {
  static const int d[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int dim = d[m - 1];
  bool leap = ((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0);
  if (m == 2 && leap) dim = 29;
  return dim;
}

static void clampEdit() {
  if (eY < 2000) eY = 2000;
  if (eY > 2099) eY = 2099;

  if (eMo < 1) eMo = 1;
  if (eMo > 12) eMo = 12;

  int dim = daysInMonth(eY, eMo);
  if (eD < 1) eD = 1;
  if (eD > dim) eD = dim;

  if (eH < 0) eH = 0;
  if (eH > 23) eH = 23;

  if (eMi < 0) eMi = 0;
  if (eMi > 59) eMi = 59;
}

static void loadFromRtc() {
  DateTime now = rtc_now();
  eY  = now.year();
  eMo = now.month();
  eD  = now.day();
  eH  = now.hour();
  eMi = now.minute();
  field = EF_YEAR;
  clampEdit();
}

// -------- draw screens
static void drawClock() {
  DateTime now = rtc_now();

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  twoDigits(now.hour()); display.print(':');
  twoDigits(now.minute()); display.print(':');
  twoDigits(now.second()); // ✅ 2 cifre

  display.setTextSize(1);
  display.setCursor(0, 28);
  twoDigits(now.day()); display.print('-');
  twoDigits(now.month()); display.print('-');
  display.print(now.year());

  display.setCursor(0, 50);
  display.println("NEXT(pin6): naeste");

  display.display();
}

static void drawDht() {
  float tC, hum;
  bool ok = dht_read(tC, hum);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Temp/Fugt (DHT11)");

  if (!ok) {
    display.setCursor(0, 18);
    display.println("DHT read error!");
  } else {
    display.setCursor(0, 18);
    display.print("Temp: "); display.print(tC, 1); display.println(" C");
    display.setCursor(0, 34);
    display.print("Fugt: "); display.print(hum, 1); display.println(" %");
  }

  display.setCursor(0, 50);
  display.println("NEXT(pin6): naeste");
  display.display();
}

static void drawSet() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Indstil tid/dato");

  if (!editMode) {
    display.setCursor(0, 16);
    display.println("Hold MINUS(pin3)=EDIT");

    display.setCursor(0, 30);
    DateTime now = rtc_now();
    display.print("Nu: ");
    twoDigits(now.day()); display.print("-");
    twoDigits(now.month()); display.print("-");
    display.print(now.year());
    display.print(" ");
    twoDigits(now.hour()); display.print(":");
    twoDigits(now.minute());

    display.setCursor(0, 50);
    display.println("NEXT(pin6): naeste");
    display.display();
    return;
  }

  display.setCursor(0, 14);
  display.println("NEXT: felt  +:pin5  -:pin3");
  display.setCursor(0, 24);
  display.println("Hold pin3: SAVE");

  // YYYY-MM-DD
  display.setCursor(0, 38);
  if (field == EF_YEAR) display.print("[");
  display.print(eY);
  if (field == EF_YEAR) display.print("]");
  display.print("-");

  if (field == EF_MONTH) display.print("[");
  twoDigits(eMo);
  if (field == EF_MONTH) display.print("]");
  display.print("-");

  if (field == EF_DAY) display.print("[");
  twoDigits(eD);
  if (field == EF_DAY) display.print("]");

  // HH:MM
  display.setCursor(0, 52);
  if (field == EF_HOUR) display.print("[");
  twoDigits(eH);
  if (field == EF_HOUR) display.print("]");
  display.print(":");
  if (field == EF_MINUTE) display.print("[");
  twoDigits(eMi);
  if (field == EF_MINUTE) display.print("]");

  display.display();
}

// -------- public API
void ui_begin() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (1) {}
  }
  display.clearDisplay();
  display.display();
}

void ui_showSplash() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("IoT House");
  display.println("pin6: NEXT/menu");
  display.println("pin5: +");
  display.println("pin3: - / hold=save");
  display.display();
  delay(900);
}

void ui_update() {
  unsigned long nowMs = millis();

  // refresh rate per screen (non-blocking)
  unsigned long interval = 200;
  if (current == SCREEN_DHT) interval = 1200; // DHT ikke for hurtigt
  if (current == SCREEN_SET) interval = 200;

  if (nowMs - lastDrawMs < interval) return;
  lastDrawMs = nowMs;

  if (current == SCREEN_CLOCK) drawClock();
  else if (current == SCREEN_DHT) drawDht();
  else drawSet();
}

ScreenId ui_screen() { return current; }

void ui_nextScreen() {
  if (editMode) return; // lås menu mens vi editer
  current = (ScreenId)((current + 1) % SCREEN_COUNT);
}

bool ui_isEditing() { return editMode; }

void ui_enterEdit() {
  if (current != SCREEN_SET) return;
  editMode = true;
  loadFromRtc();
}

void ui_exitEdit() {
  editMode = false;
}

void ui_editNextField() {
  if (!editMode) return;
  field = (EditField)((field + 1) % EF_COUNT);
}

void ui_editInc() {
  if (!editMode) return;
  switch (field) {
    case EF_YEAR:   eY++; break;
    case EF_MONTH:  eMo++; break;
    case EF_DAY:    eD++; break;
    case EF_HOUR:   eH++; break;
    case EF_MINUTE: eMi++; break;
    default: break;
  }
  clampEdit();
}

void ui_editDec() {
  if (!editMode) return;
  switch (field) {
    case EF_YEAR:   eY--; break;
    case EF_MONTH:  eMo--; break;
    case EF_DAY:    eD--; break;
    case EF_HOUR:   eH--; break;
    case EF_MINUTE: eMi--; break;
    default: break;
  }
  clampEdit();
}

void ui_editSaveExit() {
  if (!editMode) return;
  clampEdit();
  rtc_set(eY, eMo, eD, eH, eMi, 0); // seconds=0 ved save
  editMode = false;
}

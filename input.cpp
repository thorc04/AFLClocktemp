#include "input.h"
#include "ui.h"
#include <Arduino.h>

#define BTN_NEXT       6
#define BTN_PLUS       5
#define BTN_MINUS_SAVE 3

static const unsigned long DEBOUNCE_MS = 30;
static const unsigned long SAVE_HOLD_MS = 1500;

struct Button {
  int pin;
  bool lastReading;
  bool stable;
  unsigned long lastDebounce;

  bool pressed;
  unsigned long pressStart;
  bool longFired;
};

static Button bNext;
static Button bPlus;
static Button bMinus;

static void initButton(Button &b, int pin) {
  b.pin = pin;
  b.lastReading = HIGH;
  b.stable = HIGH;
  b.lastDebounce = 0;
  b.pressed = false;
  b.pressStart = 0;
  b.longFired = false;

  pinMode(b.pin, INPUT_PULLUP); // internal pullup (ingen modstand)
}

static void updateButton(Button &b) {
  unsigned long now = millis();
  bool reading = digitalRead(b.pin);

  // debounce
  if (reading != b.lastReading) {
    b.lastDebounce = now;
    b.lastReading = reading;
  }

  if (now - b.lastDebounce >= DEBOUNCE_MS) {
    if (reading != b.stable) {
      b.stable = reading;

      if (b.stable == LOW) { // pressed
        b.pressed = true;
        b.pressStart = now;
        b.longFired = false;
      } else { // released
        b.pressed = false;
      }
    }
  }

  // long press on minus/save button
  if (b.pin == BTN_MINUS_SAVE && b.pressed && !b.longFired) {
    if (now - b.pressStart >= SAVE_HOLD_MS) {
      b.longFired = true;

      if (ui_isEditing()) {
        ui_editSaveExit();   // save + exit
      } else if (ui_screen() == SCREEN_SET) {
        ui_enterEdit();      // enter edit
      }
    }
  }
}

// Detect short click (pressed -> released). We also ignore short click if longFired on minus button.
static bool shortClick(Button &b) {
  static bool prevNext = HIGH, prevPlus = HIGH, prevMinus = HIGH;

  bool *prev = &prevMinus;
  if (b.pin == BTN_NEXT) prev = &prevNext;
  else if (b.pin == BTN_PLUS) prev = &prevPlus;

  bool clicked = (*prev == LOW && b.stable == HIGH);
  *prev = b.stable;

  if (b.pin == BTN_MINUS_SAVE && b.longFired) return false;
  return clicked;
}

void input_begin() {
  initButton(bNext,  BTN_NEXT);
  initButton(bPlus,  BTN_PLUS);
  initButton(bMinus, BTN_MINUS_SAVE);
}

void input_update() {
  updateButton(bNext);
  updateButton(bPlus);
  updateButton(bMinus);

  // NEXT: menu / next field
  if (shortClick(bNext)) {
    if (ui_isEditing()) ui_editNextField();
    else ui_nextScreen();
  }

  // PLUS: +
  if (shortClick(bPlus)) {
    if (ui_isEditing()) ui_editInc();
  }

  // MINUS: -
  if (shortClick(bMinus)) {
    if (ui_isEditing()) ui_editDec();
  }
}

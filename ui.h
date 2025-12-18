#pragma once
#include <Arduino.h>

enum ScreenId { SCREEN_CLOCK, SCREEN_DHT, SCREEN_SET, SCREEN_COUNT };

void ui_begin();
void ui_showSplash();
void ui_update();

ScreenId ui_screen();
void ui_nextScreen();

bool ui_isEditing();
void ui_enterEdit();      // enter edit (kun på SCREEN_SET)
void ui_exitEdit();       // exit edit uden at gemme

void ui_editNextField();
void ui_editInc();
void ui_editDec();
void ui_editSaveExit();

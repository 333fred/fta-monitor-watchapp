#pragma once
#include <pebble.h>
#include "utility.h"

typedef enum {
  SHOW_STATUS, SHOW_NUMBERS, SHOW_BATTERY
} show_type;

void setup_text_init();
void destroy_text_init();
void setup_text_window_load(Layer *window);
void destroy_text_window_unload();
void update_text();
void set_alliance_text(const char *text, bool hi_contrast, uint8_t alliance, uint8_t team, bool useNumber);
void set_alliance_status(status_type status, uint8_t alliance, uint8_t team);

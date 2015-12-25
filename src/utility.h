#pragma once
#include <pebble.h>

typedef enum {
  ETH=0, DS=1, RADIO=2, RIO=3, CODE=4, ESTOP=5, GOOD=6, BWU=7, BYP = 8
} status_type;

char* translate_error(AppMessageResult res);
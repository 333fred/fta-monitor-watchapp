#pragma once
#include <pebble.h>

typedef enum {
  ETH=0, DS=1, RADIO=2, RIO=3, CODE=4, ESTOP=5, GOOD=6, BWU=7, BYP = 8, BAT = 9
} status_type;

typedef enum {
  NOT_READY=0, TIMEOUT=1, READY_PRESTART=2, PRESTART_INITATED=3, PRESTART_COMPLETED=4, MATCH_READY=5, AUTO=6, TELEOP=7, OVER=8, ABORTED=9
} match_state;

char* translate_error(AppMessageResult res);
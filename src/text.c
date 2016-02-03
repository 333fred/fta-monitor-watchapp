#include <pebble.h>
#include "text.h"
static TextLayer *s_red_header;
static TextLayer *s_blue_header;
static TextLayer *s_red1;
static TextLayer *s_red2;
static TextLayer *s_red3;
static TextLayer *s_blue1;
static TextLayer *s_blue2;
static TextLayer *s_blue3;
static TextLayer *s_red1_number;
static TextLayer *s_red2_number;
static TextLayer *s_red3_number;
static TextLayer *s_blue1_number;
static TextLayer *s_blue2_number;
static TextLayer *s_blue3_number;
static GFont *s_source_code_pro;
static GFont *s_source_code_pro_number;
static char *s_red1_text;
static char *s_red2_text;
static char *s_red3_text;
static char *s_blue1_text;
static char *s_blue2_text;
static char *s_blue3_text;

extern int s_red1_num;
extern int s_red2_num;
extern int s_red3_num;
extern int s_blue1_num;
extern int s_blue2_num;
extern int s_blue3_num;
extern int s_red1_battery;
extern int s_red2_battery;
extern int s_red3_battery;
extern int s_blue1_battery;
extern int s_blue2_battery;
extern int s_blue3_battery;
extern show_type s_show_type;

extern status_type s_red1_status;
extern status_type s_red2_status;
extern status_type s_red3_status;
extern status_type s_blue1_status;
extern status_type s_blue2_status;
extern status_type s_blue3_status;
  
// Constant text strings for the connection statuses
const char *eth = "Eth", *ds = "DS", *radio = "Rd", *rio = "RIO", *code = "Cd", *estop = "Est", *good = "G", *bwu = "BWU", *byp = "BYP", *bat = "Bat";

// Updates the text on the screen with the current values. It uses the correct display whether
// the current update type is status or team number.
void update_text() {
  switch (s_show_type) {
    case SHOW_STATUS:
    set_alliance_status(s_red1_status, 1, 1);
    set_alliance_status(s_red2_status, 1, 2);
    set_alliance_status(s_red3_status, 1, 3);
    set_alliance_status(s_blue1_status, 2, 1);
    set_alliance_status(s_blue2_status, 2, 2);
    set_alliance_status(s_blue3_status, 2, 3);
    break;
    case SHOW_NUMBERS:
    snprintf(s_red1_text, 5, "%d", s_red1_num);
    snprintf(s_red2_text, 5, "%d", s_red2_num);
    snprintf(s_red3_text, 5, "%d", s_red3_num);
    snprintf(s_blue1_text, 5, "%d", s_blue1_num);
    snprintf(s_blue2_text, 5, "%d", s_blue2_num);
    snprintf(s_blue3_text, 5, "%d", s_blue3_num);
    set_alliance_text(s_red1_text, false, 1, 1, true);
    set_alliance_text(s_red2_text, false, 1, 2, true);
    set_alliance_text(s_red3_text, false, 1, 3, true);
    set_alliance_text(s_blue1_text, false, 2, 1, true);
    set_alliance_text(s_blue2_text, false, 2, 2, true);
    set_alliance_text(s_blue3_text, false, 2, 3, true);
    break;
    case SHOW_BATTERY:
    snprintf(s_red1_text, 6, "%d.%d", s_red1_battery / 100, s_red1_battery % 100);
    snprintf(s_red2_text, 6, "%d.%d", s_red2_battery / 100, s_red2_battery % 100);
    snprintf(s_red3_text, 6, "%d.%d", s_red3_battery / 100, s_red3_battery % 100);
    snprintf(s_blue1_text, 6, "%d.%d", s_blue1_battery / 100, s_blue1_battery % 100);
    snprintf(s_blue2_text, 6, "%d.%d", s_blue2_battery / 100, s_blue2_battery % 100);
    snprintf(s_blue3_text, 6, "%d.%d", s_blue3_battery / 100, s_blue3_battery % 100);
    set_alliance_text(s_red1_text, false, 1, 1, true);
    set_alliance_text(s_red2_text, false, 1, 2, true);
    set_alliance_text(s_red3_text, false, 1, 3, true);
    set_alliance_text(s_blue1_text, false, 2, 1, true);
    set_alliance_text(s_blue2_text, false, 2, 2, true);
    set_alliance_text(s_blue3_text, false, 2, 3, true);
    break;
  }
}

// Sets the alliance text to be the given text string for the given alliance and team.
void set_alliance_text(const char *text, bool hi_contrast, uint8_t alliance, uint8_t team, bool useNumber) {
  uint16_t switch_mult = (alliance << 8) | team;
  TextLayer *team_layer;
  TextLayer *invisible_layer;
  switch (switch_mult) {
    case 0x0101:
    team_layer = useNumber ? s_red1_number : s_red1;
    invisible_layer = !useNumber ? s_red1_number : s_red1;
    break;
    case 0x0102:
    team_layer = useNumber ? s_red2_number : s_red2;
    invisible_layer = !useNumber ? s_red2_number : s_red2;
    break;
    case 0x0103:
    team_layer = useNumber ? s_red3_number : s_red3;
    invisible_layer = !useNumber ? s_red3_number : s_red3;
    break;
    case 0x0201:
    team_layer = useNumber ? s_blue1_number : s_blue1;
    invisible_layer = !useNumber ? s_blue1_number : s_blue1;
    break;
    case 0x0202:
    team_layer = useNumber ? s_blue2_number : s_blue2;
    invisible_layer = !useNumber ? s_blue2_number : s_blue2;
    break;
    case 0x0203:
    team_layer = useNumber ? s_blue3_number : s_blue3;
    invisible_layer = !useNumber ? s_blue3_number : s_blue3;
    break;
    default:
    return;
  }
 
  layer_set_hidden((Layer *) team_layer, false);
  layer_set_hidden((Layer *) invisible_layer, true);
  text_layer_set_text(team_layer, text);
  if (hi_contrast) {
    text_layer_set_background_color(team_layer, GColorBlack);
    text_layer_set_text_color(team_layer, GColorWhite);
  } else {
    text_layer_set_background_color(team_layer, GColorWhite);
    text_layer_set_text_color(team_layer, GColorBlack);
  }
}

// Sets the status of an alliance based on the given status type
void set_alliance_status(status_type status, uint8_t alliance, uint8_t team) {
  // If the app has told us to vibrate, then vibrate
  const char* text = NULL;
  bool hi_contrast = false;
  if (s_show_type != SHOW_STATUS) return;
  switch (status) {
    case ETH:
    text = eth;
    hi_contrast = true;
    break;
    case DS:
    text = ds;
    hi_contrast = true;
    break;
    case RADIO:
    text = radio;
    hi_contrast = true;
    break;
    case RIO:
    text = rio;
    hi_contrast = true;
    break;
    case CODE:
    text = code;
    hi_contrast = true;
    break;
    case ESTOP:
    text = estop;
    hi_contrast = true;
    break;
    case GOOD:
    text = good;
    hi_contrast = false;
    break;
    case BWU:
    text = bwu;
    hi_contrast = true;
    break;
    case BYP:
    text = byp;
    hi_contrast = false;
    break;
    case BAT:
    text = bat;
    hi_contrast = true;
    break;
  }
  set_alliance_text(text, hi_contrast, alliance, team, false);
}

void setup_alliance_textlayer(TextLayer **layer, Layer *parent, int x, int y, GFont* font) {
  *layer = text_layer_create(GRect(x, y, 74, 46));
  text_layer_set_text_alignment(*layer, GTextAlignmentCenter);
  text_layer_set_font(*layer, font);
  layer_add_child(parent, text_layer_get_layer(*layer));
}

void setup_text_window_load(Layer *window_layer) {
    // Draw the Red and Blue text for the alliance headers
  s_red_header = text_layer_create(GRect(74, 0, 74, 20));
  s_blue_header = text_layer_create(GRect(0, 0, 74, 20));
  text_layer_set_background_color(s_red_header, GColorClear);
  text_layer_set_background_color(s_blue_header, GColorClear);
  text_layer_set_text_color(s_red_header, GColorBlack);
  text_layer_set_text_color(s_blue_header, GColorBlack);
  text_layer_set_text_alignment(s_red_header, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_blue_header, GTextAlignmentCenter);
  text_layer_set_font(s_red_header, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_font(s_blue_header, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(s_red_header, "Red");
  text_layer_set_text(s_blue_header, "Blue");
  layer_add_child(window_layer, text_layer_get_layer(s_red_header));
  layer_add_child(window_layer, text_layer_get_layer(s_blue_header));
  
  // Create the layers for the different alliance statuses
  setup_alliance_textlayer(&s_red1, window_layer, 73, 112, s_source_code_pro);
  setup_alliance_textlayer(&s_red2, window_layer, 73, 66, s_source_code_pro);
  setup_alliance_textlayer(&s_red3, window_layer, 73, 20, s_source_code_pro);
  setup_alliance_textlayer(&s_blue1, window_layer, 0, 20, s_source_code_pro);
  setup_alliance_textlayer(&s_blue2, window_layer, 0, 66, s_source_code_pro);
  setup_alliance_textlayer(&s_blue3, window_layer, 0, 112, s_source_code_pro);
  
  setup_alliance_textlayer(&s_red1_number, window_layer, 73, 112, s_source_code_pro_number);
  setup_alliance_textlayer(&s_red2_number, window_layer, 73, 66, s_source_code_pro_number);
  setup_alliance_textlayer(&s_red3_number, window_layer, 73, 20, s_source_code_pro_number);
  setup_alliance_textlayer(&s_blue1_number, window_layer, 0, 20, s_source_code_pro_number);
  setup_alliance_textlayer(&s_blue2_number, window_layer, 0, 66, s_source_code_pro_number);
  setup_alliance_textlayer(&s_blue3_number, window_layer, 0, 112, s_source_code_pro_number);
  
  set_alliance_status(ETH, 1, 1);
  set_alliance_status(ETH, 1, 2);
  set_alliance_status(ETH, 1, 3);
  set_alliance_status(ETH, 2, 1);
  set_alliance_status(ETH, 2, 2);
  set_alliance_status(ETH, 2, 3);
}

void destroy_text_window_unload() {
  text_layer_destroy(s_red_header);
  text_layer_destroy(s_blue_header);
  text_layer_destroy(s_red1);
  text_layer_destroy(s_red2);
  text_layer_destroy(s_red3);
  text_layer_destroy(s_blue1);
  text_layer_destroy(s_blue2);
  text_layer_destroy(s_blue3);
  text_layer_destroy(s_red1_number);
  text_layer_destroy(s_red2_number);
  text_layer_destroy(s_red3_number);
  text_layer_destroy(s_blue1_number);
  text_layer_destroy(s_blue2_number);
  text_layer_destroy(s_blue3_number);
}

void setup_text_init() {
  s_red1_text = (char*) malloc(6 * sizeof(char));
  s_red2_text = (char*) malloc(6 * sizeof(char));
  s_red3_text = (char*) malloc(6 * sizeof(char));
  s_blue1_text = (char*) malloc(6 * sizeof(char));
  s_blue2_text = (char*) malloc(6 * sizeof(char));
  s_blue3_text = (char*) malloc(6 * sizeof(char));
  s_source_code_pro = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SOURCE_CODE_PRO_REG_38));
  s_source_code_pro_number = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SOURCE_CODE_PRO_REG_24));
}

void destroy_text_init() {
  fonts_unload_custom_font(s_source_code_pro);
  fonts_unload_custom_font(s_source_code_pro_number);
  free(s_red1_text);
  free(s_red2_text);
  free(s_red3_text);
  free(s_blue1_text);
  free(s_blue2_text);
  free(s_blue3_text);
}
#include <pebble.h>
#include "utility.h"
  
#define RED1_STATUS 1
#define RED2_STATUS 2
#define RED3_STATUS 3
#define BLUE1_STATUS 4
#define BLUE2_STATUS 5
#define BLUE3_STATUS 6
#define VIBE 7  
#define UPDATE 8
#define RED1_NUMBER 9
#define RED2_NUMBER 10
#define RED3_NUMBER 11
#define BLUE1_NUMBER 12
#define BLUE2_NUMBER 13
#define BLUE3_NUMBER 14
#define CHECK_TYPE(type, size) if (type != TUPLE_UINT && size != 1) { \
        APP_LOG(APP_LOG_LEVEL_ERROR, "Received nonuint type %d", (int) type); \
        t = dict_read_next(iter); \
        continue; \
}
  
// Constant text strings for the connection statuses
const char *eth = "Eth", *ds = "DS", *radio = "Rd", *rio = "RIO", *code = "Cd", *estop = "Est", *good = "G", *bwu = "BWU", *byp = "BYP";
  
static Window *s_main_window;
static Layer *s_grid_layer;
static TextLayer *s_red_header;
static TextLayer *s_blue_header;
static TextLayer *s_red1;
static TextLayer *s_red2;
static TextLayer *s_red3;
static TextLayer *s_blue1;
static TextLayer *s_blue2;
static TextLayer *s_blue3;
static GFont *s_source_code_pro;
static int s_red1_num = 4;
static int s_red2_num = 5;
static int s_red3_num = 6;
static int s_blue1_num = 1;
static int s_blue2_num = 2;
static int s_blue3_num = 3;
static int s_show_team_nums = 0;

static char *s_red1_text;
static char *s_red2_text;
static char *s_red3_text;
static char *s_blue1_text;
static char *s_blue2_text;
static char *s_blue3_text;

typedef enum {
  ETH=0, DS=1, RADIO=2, RIO=3, CODE=4, ESTOP=5, GOOD=6, BWU=7, BYP = 8
} status_type;

static status_type s_red1_status = 0;
static status_type s_red2_status = 0;
static status_type s_red3_status = 0;
static status_type s_blue1_status = 0;
static status_type s_blue2_status = 0;
static status_type s_blue3_status = 0;

void set_alliance_text(const char *text, bool hi_contrast, uint8_t alliance, uint8_t team);
void set_alliance_status(status_type status, uint8_t alliance, uint8_t team);

// Updates the text on the screen with the current values. It uses the correct display whether
// the current update type is status or team number.
void update_text() {
  if (s_show_team_nums) {
    snprintf(s_red1_text, 5, "%d", s_red1_num);
    snprintf(s_red2_text, 5, "%d", s_red2_num);
    snprintf(s_red3_text, 5, "%d", s_red3_num);
    snprintf(s_blue1_text, 5, "%d", s_blue1_num);
    snprintf(s_blue2_text, 5, "%d", s_blue2_num);
    snprintf(s_blue3_text, 5, "%d", s_blue3_num);
    set_alliance_text(s_red1_text, false, 1, 1);
    set_alliance_text(s_red2_text, false, 1, 2);
    set_alliance_text(s_red3_text, false, 1, 3);
    set_alliance_text(s_blue1_text, false, 2, 1);
    set_alliance_text(s_blue2_text, false, 2, 2);
    set_alliance_text(s_blue3_text, false, 2, 3);
  } else {
    set_alliance_status(s_red1_status, 1, 1);
    set_alliance_status(s_red2_status, 1, 2);
    set_alliance_status(s_red3_status, 1, 3);
    set_alliance_status(s_blue1_status, 2, 1);
    set_alliance_status(s_blue2_status, 2, 2);
    set_alliance_status(s_blue3_status, 2, 3);
  }
}

// Callback for receiving a message
static void inbox_received_callback(DictionaryIterator *iter, void *ctx) {
  Tuple *t = dict_read_first(iter);
  while (t != NULL) {
    CHECK_TYPE(t->type, t->length)
    uint32_t status = t->value->uint32;
    switch (t->key) {
      case RED1_STATUS:
      s_red1_status = (status_type) status;
      break;
      case RED2_STATUS:
      s_red2_status = (status_type) status;
      break;
      case RED3_STATUS:
      s_red3_status = (status_type) status;
      break;
      case BLUE1_STATUS:
      s_blue1_status = (status_type) status;
      break;
      case BLUE2_STATUS:
      s_blue2_status = (status_type) status;
      break;
      case BLUE3_STATUS:
      s_blue3_status = (status_type) status;
      break;
      case VIBE:
      vibes_short_pulse();
      break;
      case RED1_NUMBER:
      s_red1_num = status;
      break;
      case RED2_NUMBER:
      s_red2_num = status;
      break;
      case RED3_NUMBER:
      s_red3_num = status;
      break;
      case BLUE1_NUMBER:
      s_blue1_num = status;
      break;
      case BLUE2_NUMBER:
      s_blue2_num = status;
      break;
      case BLUE3_NUMBER:
      s_blue3_num = status;
      break;
    }
    
    t = dict_read_next(iter);
  }
  update_text();
}

// Requests an update from the companion app, if it's available
void request_update() {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, UPDATE, 0);
  app_message_outbox_send();
}

// Single click callback
void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "In single click handler");
  request_update();
}

static void message_dropped_callback(AppMessageResult reason, void *ctx) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "Message Dropped: %s", translate_error(reason));
}

static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *ctx) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "Outbox Send Failed: %s", translate_error(reason));
}

static void outbox_send_callback(DictionaryIterator *iter, void *ctx) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox Send Success!");
}

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  // Draw a line down the middle of the screen
  graphics_draw_line(ctx, GPoint(74, 0), GPoint(74, 168));
  
  // Draw the alliance line
  graphics_draw_line(ctx, GPoint(0, 20), GPoint(144, 20));
  
  // Draw the 2 actual alliance dividers. They are (168-20-10)/3 pixels apart, which is 46.
  graphics_draw_line(ctx, GPoint(0, 66), GPoint(144, 66));
  graphics_draw_line(ctx, GPoint(0, 112), GPoint(144, 112));
}

// Sets the alliance text to be the given text string for the given alliance and team.
void set_alliance_text(const char *text, bool hi_contrast, uint8_t alliance, uint8_t team) {
  uint16_t switch_mult = (alliance << 8) | team;
  TextLayer *team_layer;
  switch (switch_mult) {
    case 0x0101:
    team_layer = s_red1;
    break;
    case 0x0102:
    team_layer = s_red2;
    break;
    case 0x0103:
    team_layer = s_red3;
    break;
    case 0x0201:
    team_layer = s_blue1;
    break;
    case 0x0202:
    team_layer = s_blue2;
    break;
    case 0x0203:
    team_layer = s_blue3;
    break;
    default:
    return;
  }
 
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
  if (s_show_team_nums) return;
  switch (status) {
    case ETH:
    set_alliance_text(eth, true, alliance, team);
    break;
    case DS:
    set_alliance_text(ds, true, alliance, team);
    break;
    case RADIO:
    set_alliance_text(radio, true, alliance, team);
    break;
    case RIO:
    set_alliance_text(rio, true, alliance, team);
    break;
    case CODE:
    set_alliance_text(code, true, alliance, team);
    break;
    case ESTOP:
    set_alliance_text(estop, true, alliance, team);
    break;
    case GOOD:
    set_alliance_text(good, false, alliance, team);
    break;
    case BWU:
    set_alliance_text(bwu, true, alliance, team);
    break;
    case BYP:
    set_alliance_text(byp, false, alliance, team);
    break;
  }
}

// Down click handler - shows the team numbers for 3 seconds.
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_show_team_nums = !s_show_team_nums;
  update_text();
}

// ClickConfigProvider for the refresh button
void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void setup_alliance_textlayer(TextLayer **layer, Layer *parent, int x, int y) {
  *layer = text_layer_create(GRect(x, y, 74, 46));
  text_layer_set_text_alignment(*layer, GTextAlignmentCenter);
  text_layer_set_font(*layer, s_source_code_pro);
  layer_add_child(parent, text_layer_get_layer(*layer));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create the grid layer
  s_grid_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_add_child(window_layer, s_grid_layer);
  
  // Set the grid drawing update procedure
  layer_set_update_proc(s_grid_layer, canvas_update_proc);
  
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
  
  setup_alliance_textlayer(&s_red1, window_layer, 74, 112);
  setup_alliance_textlayer(&s_red2, window_layer, 74, 66);
  setup_alliance_textlayer(&s_red3, window_layer, 74, 20);
  setup_alliance_textlayer(&s_blue1, window_layer, 0, 20);
  setup_alliance_textlayer(&s_blue2, window_layer, 0, 66);
  setup_alliance_textlayer(&s_blue3, window_layer, 0, 112);
  set_alliance_status(ETH, 1, 1);
  set_alliance_status(ETH, 1, 2);
  set_alliance_status(ETH, 1, 3);
  set_alliance_status(ETH, 2, 1);
  set_alliance_status(ETH, 2, 2);
  set_alliance_status(ETH, 2, 3);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_grid_layer);
  text_layer_destroy(s_red_header);
  text_layer_destroy(s_blue_header);
  text_layer_destroy(s_red1);
  text_layer_destroy(s_red2);
  text_layer_destroy(s_red3);
  text_layer_destroy(s_blue1);
  text_layer_destroy(s_blue2);
  text_layer_destroy(s_blue3);
}

static void init() {
  s_red1_text = (char*) malloc(5 * sizeof(char));
  s_red2_text = (char*) malloc(5 * sizeof(char));
  s_red3_text = (char*) malloc(5 * sizeof(char));
  s_blue1_text = (char*) malloc(5 * sizeof(char));
  s_blue2_text = (char*) malloc(5 * sizeof(char));
  s_blue3_text = (char*) malloc(5 * sizeof(char));
  s_source_code_pro = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SOURCE_CODE_PRO_REG_38));
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  
  // Set up AppMessage so we are ready to receive data from the phone
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(message_dropped_callback);
  app_message_register_outbox_sent(outbox_send_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  request_update();
  window_set_click_config_provider(s_main_window, (ClickConfigProvider) config_provider);
}

static void deinit() {
  window_destroy(s_main_window);
  fonts_unload_custom_font(s_source_code_pro);
  app_message_deregister_callbacks();
  free(s_red1_text);
  free(s_red2_text);
  free(s_red3_text);
  free(s_blue1_text);
  free(s_blue2_text);
  free(s_blue3_text);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
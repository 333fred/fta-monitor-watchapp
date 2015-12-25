#include <pebble.h>
#include "utility.h"
#include "text.h"
  
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

static Window *s_main_window;
static Layer *s_grid_layer;
int s_red1_num = 4;
int s_red2_num = 5;
int s_red3_num = 6;
int s_blue1_num = 1;
int s_blue2_num = 2;
int s_blue3_num = 3;
int s_show_team_nums = 0;

status_type s_red1_status = 0;
status_type s_red2_status = 0;
status_type s_red3_status = 0;
status_type s_blue1_status = 0;
status_type s_blue2_status = 0;
status_type s_blue3_status = 0;

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

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create the grid layer
  s_grid_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_add_child(window_layer, s_grid_layer);
  
  // Set the grid drawing update procedure
  layer_set_update_proc(s_grid_layer, canvas_update_proc);
  
  setup_text_window_load(window_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_grid_layer);
  destroy_text_window_unload();
}

static void init() {
  setup_text_init();
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
  app_message_deregister_callbacks();
  destroy_text_init();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
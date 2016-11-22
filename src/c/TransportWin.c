
#include <pebble.h>
#include "timerWin.h"
#include "src/c/bitmap-loader.h"
#include "icons.h"
#include "src/c/timer.h"
#include "src/c/timers.h"
#include "Activities.h"
#include "src/c/setting.h"

#define  ACTIVITY_KEY 1

#define  TIMER_KEY 1
#define  TIMER_DEFAULT 0

#define  pause_transport_KEY 2
#define  pause_transport_DEFAULT false

#define  UNIX_TIME_KEY 3 
#define  UNIX_TIME_DEFAULT 0

#define  time_end_transport_KEY 4 
#define  time_end_transport_DEFAULT 0
  
static Window *s_main_window;
static TextLayer *s_label_layer;
static BitmapLayer *s_icon_layer;
static ActionBarLayer *s_action_bar_layer;
//static Timer* s_timer;


static GBitmap *s_icon_bitmap, *s_pause_bitmap, *s_cross_bitmap, *s_play_bitmap;

char tmp_transport_ready[16];
uint32_t time_stopwatch_transport = TIMER_DEFAULT;
bool pause_transport = pause_transport_DEFAULT;

uint32_t time_begin_transport, time_end_transport, time_elapse_transport;


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)time_end_transport);
   // Begin dictionary  
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "I've clicked");
  

}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  pause_transport = !pause_transport;
  
  //when it is pause_transportd, it logs the beginning time and the end time for this session and send the data to firebase
  if(pause_transport){
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_play_bitmap);
    time_end_transport = (uint32_t)time(NULL);
    time_elapse_transport = time_end_transport - time_begin_transport;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "time_begin_transport: %d time_end_readFy %d", (int)time_begin_transport, (int)time_end_transport);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "duration: %d ", (int)time_elapse_transport);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "messagekey begin: %d ", (int)MESSAGE_KEY_BEGIN);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "messagekey end: %d ", (int)MESSAGE_KEY_END);
    
    //beginning to encode and send time data
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    dict_write_int32(iter, MESSAGE_KEY_BEGIN, (int)time_begin_transport);
    dict_write_int32(iter, MESSAGE_KEY_END, (int)time_end_transport);
    dict_write_cstring(iter, MESSAGE_KEY_ACTIVITY, "transport");
    
    app_message_outbox_send();

  }else {
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_pause_bitmap);
    time_begin_transport = (uint32_t)time(NULL);
  }
    
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  pause_transport = true;
  time_end_transport = (uint32_t)time(NULL);
  
  time_stopwatch_transport = TIMER_DEFAULT;
  
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void update_time() {
  if(!pause_transport){
    time_stopwatch_transport++;
  }
  
  timer_time_str(time_stopwatch_transport, settings()->timers_hours, tmp_transport_ready, sizeof(tmp_transport_ready));
  
  text_layer_set_text(s_label_layer, tmp_transport_ready );
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void window_load(Window *window) {
  
  time_begin_transport = (uint32_t)time(NULL);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "The current time is %d", (int)time_begin_transport);
  pause_transport = persist_exists(pause_transport_KEY) ? persist_read_bool(pause_transport_KEY)  :  pause_transport_DEFAULT;
  time_end_transport = persist_exists(time_end_transport_KEY) ? persist_read_int(time_end_transport_KEY)  : time_end_transport_DEFAULT;
  uint32_t time_elapse_transport; 
  if (!pause_transport){
    time_elapse_transport = time_begin_transport - time_end_transport;
  } else {
    time_elapse_transport = 0;
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "The time_elapse_transport  is %d", (int)time_elapse_transport);
  
  if (persist_exists(TIMER_KEY)){
    time_stopwatch_transport = time_elapse_transport + persist_read_int(TIMER_KEY);
  } else{
    time_stopwatch_transport = TIMER_DEFAULT;
  }
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLEEP);

  const GEdgeInsets icon_insets = {.top = 7, .right = 28, .bottom = 56, .left = 14};
  s_icon_layer = bitmap_layer_create(grect_inset(bounds, icon_insets));
  bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
  bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_icon_layer));

  const GEdgeInsets label_insets = {.top = 112, .right = ACTION_BAR_WIDTH, .left = ACTION_BAR_WIDTH / 2};
  s_label_layer = text_layer_create(grect_inset(bounds, label_insets));
  
  //time updates here:
  text_layer_set_text(s_label_layer, tmp_transport_ready );
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text_alignment(s_label_layer, GTextAlignmentCenter);
  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

  s_pause_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PAUSE);
  s_cross_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CROSS);
  s_play_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PLAY);
  
  s_action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_click_config_provider(s_action_bar_layer, click_config_provider);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_pause_bitmap);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_DOWN, s_cross_bitmap);
  action_bar_layer_add_to_window(s_action_bar_layer, window);
  
  //updating the time
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
    // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
}

static void window_unload(Window *window) {
  persist_write_bool(pause_transport_KEY, pause_transport);
  persist_write_int(TIMER_KEY, time_stopwatch_transport);
  
  if (!pause_transport){
    persist_write_int(time_end_transport_KEY, time_end_transport = time_begin_transport);
  }
  
  text_layer_destroy(s_label_layer);
  action_bar_layer_destroy(s_action_bar_layer);
  bitmap_layer_destroy(s_icon_layer);

  gbitmap_destroy(s_icon_bitmap);
  gbitmap_destroy(s_pause_bitmap);
  gbitmap_destroy(s_cross_bitmap);

  window_destroy(window);
  s_main_window = NULL;
}

void transport_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_background_color(s_main_window, PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorWhite));
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    
    app_message_register_outbox_sent(outbox_sent_callback);

    
  }
  window_stack_push(s_main_window, true);
}

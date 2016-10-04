
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

#define  PAUSE_KEY 2
#define  PAUSE_DEFAULT false

#define  UNIX_TIME_KEY 3 
#define  UNIX_TIME_DEFAULT 0

#define  TIME_END_KEY 4 
#define  TIME_END_DEFAULT 0
  
static Window *s_main_window;
static TextLayer *s_label_layer;
static BitmapLayer *s_icon_layer;
static ActionBarLayer *s_action_bar_layer;
static Timer* s_timer;


static GBitmap *s_icon_bitmap, *s_pause_bitmap, *s_cross_bitmap, *s_play_bitmap;

char tmp[16];
uint32_t time_stopwatch = TIMER_DEFAULT;
bool pause = PAUSE_DEFAULT;

uint32_t time_begin, time_end, time_elapse;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)time_end);


}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  pause = !pause;

  if(pause){
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_play_bitmap);
    time_end = (uint32_t)time(NULL);
    time_elapse = time_end - time_begin;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "time_begin: %d time_end %d", (int)time_begin, (int)time_end);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "duration: %d ", (int)time_elapse);

  }else {
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_pause_bitmap);
    time_begin = (uint32_t)time(NULL);
  }
    
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  pause = true;
  time_end = (uint32_t)time(NULL);
  
  //copied from the up handler, doesn't work.
//   time_elapse = time_end - time_begin;
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "time_begin: %d time_end %d", (int)time_begin, (int)time_end);
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "duration: %d ", (int)time_elapse);
  time_stopwatch = TIMER_DEFAULT;
  
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void update_time() {
  if(!pause){
    time_stopwatch++;
  }
  
  timer_time_str(time_stopwatch, settings()->timers_hours, tmp, sizeof(tmp));
  
  text_layer_set_text(s_label_layer, tmp );
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_load(Window *window) {
  
  time_begin = (uint32_t)time(NULL);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "The current time is %d", (int)time_begin);
  pause = persist_exists(PAUSE_KEY) ? persist_read_bool(PAUSE_KEY)  :  PAUSE_DEFAULT;
  time_end = persist_exists(TIME_END_KEY) ? persist_read_int(TIME_END_KEY)  : TIME_END_DEFAULT;
  uint32_t time_elapse; 
  if (!pause){
    time_elapse = time_begin - time_end;
  } else {
    time_elapse = 0;
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "The time_elapse  is %d", (int)time_elapse);
  
  if (persist_exists(TIMER_KEY)){
    time_stopwatch = time_elapse + persist_read_int(TIMER_KEY);
  } else{
    time_stopwatch = TIMER_DEFAULT;
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
  text_layer_set_text(s_label_layer, tmp );
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
}

static void window_unload(Window *window) {
  persist_write_bool(PAUSE_KEY, pause);
  persist_write_int(TIMER_KEY, time_stopwatch);
  
  if (!pause){
    persist_write_int(TIME_END_KEY, time_end = time_begin);
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

void dialog_choice_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_background_color(s_main_window, PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorWhite));
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}

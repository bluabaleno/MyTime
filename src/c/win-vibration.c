

#include <pebble.h>

#include "src/c/pebble-assist.h"
#include "src/c/bitmap-loader.h"
#include "win-vibration.h"

#include "timer.h"
#include "common.h"
#include "icons.h"

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data);
static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window* window;
static MenuLayer* layer_menu;
static VibrationCallback current_callback;
static TimerVibration current_vibration;

void win_vibration_init(void) {
  window = window_create();

  layer_menu = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(layer_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback,
  });
  menu_layer_set_click_config_onto_window(layer_menu, window);
  menu_layer_add_to_window(layer_menu, window);
}

void win_vibration_show(VibrationCallback callback, TimerVibration vibration) {
  window_stack_push(window, true);
  MenuIndex index;
  index.section = 0;
  index.row = vibration;
  menu_layer_set_selected_index(layer_menu, index, MenuRowAlignTop, false);
  current_vibration = vibration;
  current_callback = callback;
}

void win_vibration_destroy(void) {
  menu_layer_destroy(layer_menu);
  window_destroy(window);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 6;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *data) {
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* me, MenuIndex* cell_index, void* data) {
  return 36;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  char label[24];
  strcpy(label, timer_vibe_str(cell_index->row, false));
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(4, 1, 112, 28), GTextOverflowModeTrailingEllipsis , GTextAlignmentLeft, NULL);
  if (current_vibration == cell_index->row) {
    graphics_draw_bitmap_in_rect(ctx, bitmaps_get_sub_bitmap(RESOURCE_ID_ICONS_16, ICON_RECT_DONE), GRect(120, 10, 16, 16));
  }

}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  current_callback(cell_index->row);
  window_stack_pop(true);
}
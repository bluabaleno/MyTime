/**
 * Example implementation of the list menu UI pattern.
 */

#include "Activities.h"
#include "src/c/timerWin.h"
#include "timer.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_list_message_layer;
static Timer* s_timer = NULL;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return LIST_MESSAGE_WINDOW_NUM_ROWS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "Sleep", NULL, NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Getting ready", NULL, NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "Transport", NULL, NULL);
      break;
    case 3:
      menu_cell_basic_draw(ctx, cell_layer, "Eat", NULL, NULL);
      break;
    case 4:
      menu_cell_basic_draw(ctx, cell_layer, "Swimming", NULL, NULL);
      break;
    case 5:
      menu_cell_basic_draw(ctx, cell_layer, "Programming", NULL, NULL);
      break;
    case 6:
      menu_cell_basic_draw(ctx, cell_layer, "Writing", NULL, NULL);
      break;
    case 7:
      menu_cell_basic_draw(ctx, cell_layer, "Higher Mathematics", NULL, NULL);
      break;
    case 8:
      menu_cell_basic_draw(ctx, cell_layer, "Time management", NULL, NULL);
      break;
    case 9:
      menu_cell_basic_draw(ctx, cell_layer, "Planning", NULL, NULL);
      break;
    case 10:
      menu_cell_basic_draw(ctx, cell_layer, "Recovery", NULL, NULL);
      break;
    default:
      break;
  }
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      dialog_choice_window_push();
      s_timer = timer_create_timer();
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    case 9:
      break;
    case 10:
      break;
    default:
      break;
  }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    LIST_MESSAGE_WINDOW_CELL_HEIGHT);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
#if defined(PBL_COLOR)
  menu_layer_set_normal_colors(s_menu_layer, GColorBlack, GColorWhite);
  menu_layer_set_highlight_colors(s_menu_layer, GColorRed, GColorWhite);
#endif
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
  
  const GEdgeInsets message_insets = {.top = 140};
  s_list_message_layer = text_layer_create(grect_inset(bounds, message_insets));
  text_layer_set_text_alignment(s_list_message_layer, GTextAlignmentCenter);
  text_layer_set_text(s_list_message_layer, LIST_MESSAGE_WINDOW_HINT_TEXT);
  layer_add_child(window_layer, text_layer_get_layer(s_list_message_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  text_layer_destroy(s_list_message_layer);

  window_destroy(window);
  s_main_window = NULL;
}

void list_message_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}

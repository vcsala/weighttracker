#include <pebble.h>
#include "../support/config.h"
#include "../support/reminder.h"
#include "../support/utils.h"
#include "../components/selection_layer.h"
#include "../windows/days.h"
#include "time.h"

const int NUM_OF_CELLS = 2;
const int CELL_PADDING = 10;

static Window *s_window;
//static GFont s_res_title;
//static TextLayer *s_titlelayer;
static Layer *s_timelayer;

static char s_hour_str[3] = "";
static char s_minute_str[3] = "";

static bool s_left = false;

static char* selection_handle_get_text(int index, void *context) {
  switch (index) {
    case 0:
      zero_padded_int_str((get_reminder_ptr())->hour, s_hour_str, 3);
      return s_hour_str;
      break;

    default:
      zero_padded_int_str((get_reminder_ptr())->minute, s_minute_str, 3);
      return s_minute_str;
      break;

    /* default:
      if ((get_reminder_ptr())->before_noon) {
        strcpy(s_period, "AM");
      } else {
        strcpy(s_period, "PM");
      }

      return s_period;
      break; */
  }
}

static void selection_handle_complete(void *context) {
  s_left = true;
  show_days();
}

static void selection_handle_dec(int index, uint8_t clicks, void *context) {
  switch (index) {
    case 0:
      (get_reminder_ptr())->hour = ((get_reminder_ptr())->hour + 23) % 24;
      break;

    default:
      (get_reminder_ptr())->minute = ((get_reminder_ptr())->minute + 59) % 60;
      break;
  }
}

static void selection_handle_inc(int index, uint8_t clicks, void *context) {
  switch (index) {
    case 0:
      (get_reminder_ptr())->hour = ((get_reminder_ptr())->hour + 1) % 24;
      break;

    default:
      (get_reminder_ptr())->minute = ((get_reminder_ptr())->minute + 1) % 60;
      break;
  }
}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, BACKGROUND);
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  //s_res_title = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

  // s_titlelayer
  /*
  s_titlelayer = text_layer_create(GRect(0, 0, bounds.size.w, 28));
  text_layer_set_background_color(s_titlelayer, GColorClear);
  text_layer_set_text_color(s_titlelayer, FOREGROUND);
  text_layer_set_text(s_titlelayer, "Time");
  text_layer_set_text_alignment(s_titlelayer, GTextAlignmentCenter);
  text_layer_set_font(s_titlelayer, s_res_title);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_titlelayer);
  */

  s_timelayer = selection_layer_create(GRect(CELL_PADDING, bounds.size.h / 2 - 17, bounds.size.w - CELL_PADDING * (NUM_OF_CELLS - 1), 34), NUM_OF_CELLS);

  for (int i = 0; i < NUM_OF_CELLS; i++) {
    selection_layer_set_cell_width(s_timelayer, i, (bounds.size.w - CELL_PADDING * (NUM_OF_CELLS + 1)) / NUM_OF_CELLS);
  }

  selection_layer_set_cell_padding(s_timelayer, CELL_PADDING);
  selection_layer_set_active_bg_color(s_timelayer, HIGHLIGHT);
  selection_layer_set_inactive_bg_color(s_timelayer, INACTIVECELL);
  selection_layer_set_active_fg_color(s_timelayer, ACTIVETEXT);
  selection_layer_set_inactive_fg_color(s_timelayer, INACTIVETEXT);
  selection_layer_set_click_config_onto_window(s_timelayer, s_window);
  selection_layer_set_callbacks(s_timelayer, NULL, (SelectionLayerCallbacks) {
    .get_cell_text = selection_handle_get_text,
    .complete = selection_handle_complete,
    .increment = selection_handle_inc,
    .decrement = selection_handle_dec,
  });
  layer_add_child(window_get_root_layer(s_window), s_timelayer);
/*
#ifdef PBL_ROUND
  text_layer_enable_screen_text_flow_and_paging(s_titlelayer, 4);
#endif
*/
}

static void destroy_ui(void) {
  window_destroy(s_window);
  //text_layer_destroy(s_titlelayer);
  selection_layer_destroy(s_timelayer);
}

static void handle_menu_window_appear(Window *window) {
  if (s_left) {
    selection_layer_set_selected(s_timelayer, NUM_OF_CELLS - 1);
  }
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_time(void) {
  s_left = false;

  initialise_ui();

  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
    .appear = handle_menu_window_appear,
  });

  window_stack_push(s_window, true);
}

void hide_time(bool animate) {
  window_stack_remove(s_window, animate);
}

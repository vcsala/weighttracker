#include <pebble.h>
#include "../support/config.h"
#include "../windows/wakeup.h"

#define INSTRUCTION_HEIGHT PBL_IF_ROUND_ELSE(40, 34)
#define MESSAGE_HEIGHT 68
#define CANVAS_HEIGHT 32
#define CANVAS_WIDTH 64

static Window *s_window;
static TextLayer *s_textlayer_1;
static TextLayer *s_textlayer_2;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, BACKGROUND);
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_frame(window_layer);

  s_textlayer_1 = text_layer_create(GRect(0, (bounds.size.h - MESSAGE_HEIGHT) / 2, bounds.size.w, MESSAGE_HEIGHT));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text_color(s_textlayer_1, CHARTCOLOR);
  text_layer_set_text(s_textlayer_1, "It is time to measure yourself and record your weight!");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, (Layer *)s_textlayer_1);

  s_textlayer_2 = text_layer_create(GRect(0, bounds.size.h - INSTRUCTION_HEIGHT, bounds.size.w, INSTRUCTION_HEIGHT));
  text_layer_set_background_color(s_textlayer_2, GColorClear);
  text_layer_set_text_color(s_textlayer_2, FOREGROUND);
  text_layer_set_text(s_textlayer_2, "Push Back button for main window");
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_2, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_layer, (Layer *)s_textlayer_2);

#ifdef PBL_ROUND
  text_layer_enable_screen_text_flow_and_paging(s_textlayer_2, 4);
  text_layer_enable_screen_text_flow_and_paging(s_textlayer_1, 4);
#endif
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_textlayer_2);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_wakeup(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
  vibes_long_pulse();
}

void hide_wakeup(void) {
  window_stack_remove(s_window, true);
}

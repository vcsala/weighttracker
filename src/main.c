// ORIGINAL_UUID "7151ed69-3204-4914-8aaa-860e1494a504"
// TEST_UUID "4d2f7dd6-2b26-4cce-ace5-410741b853bb"

#include <pebble.h>
#include "support/config.h"
#include "support/utils.h"
#include "support/draw.h"
#include "support/data.h"
#include "support/pin.h"
#include "support/parameters.h"
#include "windows/settings.h"
#include "windows/details.h"
#include "windows/input.h"
#include "windows/welcome.h"
#include "windows/wakeup.h"
#include "main.h"

static int s_today;

static Window *s_window;
static TextLayer *s_textlayer_1;
static TextLayer *s_textlayer_2;
static Layer *s_chart;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, BACKGROUND);
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_frame(window_layer);

  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(14, 0), bounds.size.w, PBL_IF_ROUND_ELSE(24, 28)));
  text_layer_set_background_color(s_textlayer_2, GColorClear);
  text_layer_set_text_color(s_textlayer_2, FOREGROUND);
  text_layer_set_text(s_textlayer_2, "WeightTracker");
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_2, fonts_get_system_font(PBL_IF_ROUND_ELSE(FONT_KEY_GOTHIC_14_BOLD, FONT_KEY_GOTHIC_24_BOLD)));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);

  // s_chart
  s_chart = layer_create(GRect(PBL_IF_ROUND_ELSE((bounds.size.w - 144) / 2 - 4, 0), 40, 144, 110));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_chart);

  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(0, bounds.size.h - PBL_IF_ROUND_ELSE(38, 20), bounds.size.w, PBL_IF_ROUND_ELSE(38, 24)));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text_color(s_textlayer_1, FOREGROUND);
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);

#ifdef PBL_ROUND
  text_layer_enable_screen_text_flow_and_paging(s_textlayer_2, 2);
  text_layer_enable_screen_text_flow_and_paging(s_textlayer_1, 2);
#endif
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_2);
  layer_destroy(s_chart);
  text_layer_destroy(s_textlayer_1);
}

static void wakeup_handler(/* WakeupId id, int32_t reason */) {
  show_wakeup();
}

void display_info(void) {
  static char date[16];
  static char weight[11];
  static char info[31];
  uint32_t cdt = get_latest_date();

  if (has_used_already() && cdt > 0) {
    date_str(cdt, date, 15);
    double_str(to_current_unit(get_latest_weight()), weight, 10);
    snprintf(info, sizeof(info), "Last: %s - %s %s", date, weight, get_short_unit_name());
  } else {
    snprintf(info, sizeof(info), "No measurement yet");
  }

  text_layer_set_text(s_textlayer_1, info);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (tick_time->tm_yday != s_today) {
    move_back();
    layer_mark_dirty(s_chart);
    display_info();
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_settings();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_input();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  show_details();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

#ifndef PBL_PLATFORM_APLITE
static double anim_percentage(AnimationProgress dist_normalized, double max) {
  return (((double)dist_normalized / (double)ANIMATION_NORMALIZED_MAX) * max);
}

static void height_update(Animation *anim, AnimationProgress dist_normalized) {
  set_multiplier(anim_percentage(dist_normalized, 1.0));

  layer_mark_dirty(s_chart);
}
#endif

static void handle_window_unload(Window* window) {
  destroy_ui();
}

static void handle_window_appear(Window* window) {
  display_info();
}

static void show_main(void) {
  initialise_ui();

  layer_set_update_proc(s_chart, draw_chart_callback);

  display_info();

  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
    .appear = handle_window_appear
  });

  tick_timer_service_subscribe(HOUR_UNIT, tick_handler);

  window_stack_push(s_window, true);
}

static void hide_main(void) {
  tick_timer_service_unsubscribe();
  window_stack_remove(s_window, true);
}

void init() {
  pin_initialize();
  s_today = get_year_day();

#if __TESTING == 1
  test_data();
#endif

  load_data();
  wakeup_service_subscribe(wakeup_handler);
}

static void deinit() {
  save_data();
  pin_deinitalize();
}

#if __TESTING == 1
char *translate_launch_reason(AppLaunchReason reason) {
  switch (reason) {
    case APP_LAUNCH_SYSTEM: return "APP_LAUNCH_SYSTEM";
    case APP_LAUNCH_USER: return "APP_LAUNCH_USER";
    case APP_LAUNCH_WAKEUP: return "APP_LAUNCH_WAKEUP";
    case APP_LAUNCH_PHONE: return "APP_LAUNCH_PHONE";
    case APP_LAUNCH_WORKER: return "APP_LAUNCH_WORKER";
    case APP_LAUNCH_QUICK_LAUNCH: return "APP_LAUNCH_QUICK_LAUNCH";
    case APP_LAUNCH_TIMELINE_ACTION: return "APP_LAUNCH_TIMELINE_ACTION";
    default: return "UNKNOWN REASON";
  }
}
#endif

int main(void) {
#ifndef PBL_PLATFORM_APLITE
  bool do_anim = true;
#endif

  init();

  show_main();

#if __TESTING == 1
  LOG_VERSION();
#endif

  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    show_wakeup();
#ifndef PBL_PLATFORM_APLITE
    do_anim = false;
#endif
  } else if (get_stored_version() != get_current_version()) {
    show_welcome();
#ifndef PBL_PLATFORM_APLITE
    do_anim = false;
#endif
  }

#ifndef PBL_PLATFORM_APLITE
  if (do_anim) {
    set_multiplier(0.0);

    // Prepare animations
    AnimationImplementation height_impl = {
      .update = height_update
    };
    animate(ANIMATION_DURATION, ANIMATION_DELAY, &height_impl, false);
  } else {
    set_multiplier(1.0);
  }
#else
  set_multiplier(1.0);
#endif

  app_event_loop();
  hide_main();
  deinit();
}

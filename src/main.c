#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_text_ger_layer;
static TextLayer *s_time_ger_layer;
static TextLayer *s_battery_layer;
static void handle_accel_tap(AccelAxisType axis, int32_t direction);
static bool is_hidden;
static TextLayer *s_time_layer, *s_date_layer;
AppTimer * timer_hide;

int current_color = 0;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  time_t temp2 = time(NULL)+(6*60*60);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char s_buffer2[8];
  static char s_buffer3[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  //strftime(s_buffer3, sizeof(s_buffer3), "%H:%M", tick_time);
  
  struct tm *zulu_time = localtime(&temp2);
  strftime(s_buffer3, sizeof(s_buffer3), "%H:%M", zulu_time);
  
  
  
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
  text_layer_set_text(s_date_layer, date_buffer);

  
  
  
  

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_time_ger_layer, s_buffer3);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void battery_handler(BatteryChargeState charge_state) {
  static char s_battery_buffer[16];

  if (charge_state.is_charging) {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "chrg");
  } else {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(GRect(0, 58, bounds.size.w, 50));
  
  window_set_background_color(window, GColorDukeBlue);
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorYellow);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  s_battery_layer = text_layer_create(GRect(0, 0, 36, 20));
  layer_set_hidden((Layer *) s_battery_layer, true);
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  text_layer_set_background_color(s_battery_layer, GColorClear);  
  text_layer_set_text_color(s_battery_layer, GColorYellow);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  
  battery_handler(battery_state_service_peek());

  ///
  s_text_ger_layer = text_layer_create(GRect(0, 134, 46, 14));
  layer_set_hidden((Layer *) s_text_ger_layer, true);
  text_layer_set_text(s_text_ger_layer, "GER");
  text_layer_set_background_color(s_text_ger_layer, GColorClear);
  text_layer_set_text_color(s_text_ger_layer, GColorYellow);
  layer_add_child(window_layer, text_layer_get_layer(s_text_ger_layer));
  text_layer_set_text_alignment(s_text_ger_layer, GTextAlignmentCenter);
  
  ///
  s_time_ger_layer = text_layer_create(GRect(0, 148, 46, 20));
  layer_set_hidden((Layer *) s_time_ger_layer, true);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_background_color(s_time_ger_layer, GColorClear);
  text_layer_set_text_color(s_time_ger_layer, GColorYellow);
  layer_add_child(window_layer, text_layer_get_layer(s_time_ger_layer));
  text_layer_set_font(s_time_ger_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_time_ger_layer, GTextAlignmentCenter);

    ///

  
  ////
  s_date_layer = text_layer_create(GRect(0, 100, 144, 30));
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}






static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  battery_state_service_subscribe(battery_handler);
  accel_tap_service_subscribe(&handle_accel_tap);
  
  is_hidden = false;
}

static void hideCallback(void *data) {
 is_hidden = !is_hidden;
 layer_set_hidden((Layer *)s_battery_layer, !is_hidden);
 layer_set_hidden((Layer *)s_time_ger_layer, !is_hidden);
 layer_set_hidden((Layer *)s_text_ger_layer, !is_hidden);
  
}


static void handle_accel_tap(AccelAxisType axis, int32_t direction)
{
 // vibes_short_pulse(); 
 hideCallback(NULL); 
  timer_hide =  app_timer_register(2000,(AppTimerCallback) hideCallback, NULL);
}





static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
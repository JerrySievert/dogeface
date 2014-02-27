#include <pebble.h>

static Window *window;
static TextLayer *text_time_layer;
static TextLayer *text_price_layer;
static GFont *font24;
static GFont *font18;

static GBitmap *image = NULL;
static BitmapLayer *image_layer;

enum {
  SATOSHIS
};

void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
  Tuple *satoshis = dict_find(received, SATOSHIS);

  if (satoshis) {
    text_layer_set_text(text_price_layer, satoshis->value->cstring);
  }
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";

  char *time_format;


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);
}

static void window_load(Window *window) {
  font24 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_24));
  font18 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_18));

  Layer *window_layer = window_get_root_layer(window);

  // add the doge face
  image = gbitmap_create_with_resource(RESOURCE_ID_DOGEFACE);
  image_layer = bitmap_layer_create(GRect(8, 0, 132, 116));
  bitmap_layer_set_bitmap(image_layer, image);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));

  text_time_layer = text_layer_create(GRect(12, 120, 132, 144));
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_time_layer, GColorBlack);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, font24);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  text_price_layer = text_layer_create(GRect(12, 146, 132, 167));
  text_layer_set_text_alignment(text_price_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_price_layer, GColorBlack);
  text_layer_set_background_color(text_price_layer, GColorClear);
  text_layer_set_font(text_price_layer, font18);
  layer_add_child(window_layer, text_layer_get_layer(text_price_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_price_layer);

  gbitmap_destroy(image);
  layer_remove_from_parent(bitmap_layer_get_layer(image_layer));
  bitmap_layer_destroy(image_layer);

  fonts_unload_custom_font(font18);
  fonts_unload_custom_font(font24);
}

static void app_message_init(void) {
  app_message_open(64, 16);
  app_message_register_inbox_received(in_received_handler);
}

static void init(void) {
  window = window_create();
  app_message_init();

  window_set_background_color(window, GColorWhite);

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  // subscribe to update every minute
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  app_event_loop();
  deinit();
}

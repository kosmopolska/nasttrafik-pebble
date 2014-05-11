#include <pebble.h>

Window* window;
TextLayer *title_layer, *line_layer, *times_layer;
ScrollLayer *scroll_layer;

char line_buffer[1024], times_buffer[1024], title_buffer[32];
char line_display[1024], times_display[1024];

enum
{
	KEY_LINES = 0,
  KEY_TIMES = 1,
  KEY_STATION_NAME = 2,
  KEY_DONE = 3,
  KEY_NO_OF_DEPARTURES = 4,

};

void process_tuple(Tuple *t)
{
  //Get key
  int key = t->key;

  //Get integer value, if present
  int value = t->value->int32;

  //Get string value, if present
  char string_value[32];
  strcpy(string_value, t->value->cstring);

  //Decide what to do
  switch(key) {
    case KEY_LINES:
      //Lines received
      strcat(line_buffer, string_value);
      strcat(line_buffer, "\n");
      break;
    case KEY_TIMES:
      //Times received
      strcat(times_buffer, string_value);
      strcat(times_buffer, "\n");
      break;
    case KEY_STATION_NAME:
      //Station name received
      snprintf(title_buffer, 32, "%s", string_value);
      text_layer_set_text(title_layer, (char*) &title_buffer);
      break;
    case KEY_DONE:
      strcpy(line_display, line_buffer);
      strcpy(times_display, times_buffer);
      text_layer_set_text(line_layer, (char*) &line_display);
      text_layer_set_text(times_layer, (char*) &times_display);
      strcpy(line_buffer, "");
      strcpy(times_buffer, "");
      break;
    case KEY_NO_OF_DEPARTURES:
      text_layer_set_size(line_layer, GSize(100, value*30));
      text_layer_set_size(times_layer, GSize(140, value*30));
      break;
  }
}

void send_int(uint8_t key, uint8_t cmd)
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  Tuplet value = TupletInteger(key, cmd);
  dict_write_tuplet(iter, &value);

  app_message_outbox_send();
}

void tick_callback(struct tm *tick_time, TimeUnits units_changed)
{
  send_int(5, 5);
}

static void in_received_handler(DictionaryIterator *iter, void *context)
{
	//Get data
  Tuple *t = dict_read_first(iter);
  if(t)
  {
    process_tuple(t);
  }

  //Get next
  while(t != NULL)
  {
    t = dict_read_next(iter);
    if(t)
    {
      process_tuple(t);
    }
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Out failed: %i", reason);
}

static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
  TextLayer *layer = text_layer_create(location);
  text_layer_set_text_color(layer, colour);
  text_layer_set_background_color(layer, background);
  text_layer_set_font(layer, fonts_get_system_font(res_id));
  text_layer_set_text_alignment(layer, alignment);

  return layer;
}


void window_load(Window *window)
{
  ScrollLayer *scroll_layer = scroll_layer_create(GRect(0, 0, 144, 168));
  //layer_add_child(window_get_root_layer(window), scroll_layer_get_layer(scroll_layer));
  scroll_layer_set_click_config_onto_window(scroll_layer, window);
  scroll_layer_set_content_size(scroll_layer, GSize(144,168));

	title_layer = init_text_layer(GRect(0, 0, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
  text_layer_set_text(title_layer, "Laddar…");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(title_layer));

  line_layer = init_text_layer(GRect(5, 20, 100, 90), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
  text_layer_set_text(line_layer, "");
  //text_layer_set_overflow_mode(line_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(line_layer));

  times_layer = init_text_layer(GRect(0, 20, 140, 90), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentRight);
  text_layer_set_text(times_layer, "");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(times_layer));

}

void window_unload(Window *window)
{
  text_layer_destroy(title_layer);
  text_layer_destroy(line_layer);
  text_layer_destroy(times_layer);
  scroll_layer_destroy(scroll_layer);
}

void init()
{
  window = window_create();
  WindowHandlers handlers = {
    .load = window_load,
    .unload = window_unload
  };
  window_set_window_handlers(window, handlers);

	//Register AppMessage events
	app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());   //Large input and output buffer sizes
  tick_timer_service_subscribe(MINUTE_UNIT, tick_callback);

  window_stack_push(window, true);

  strcpy(line_buffer, "");
  strcpy(times_buffer, "");
}

void deinit()
{
  window_destroy(window);
  tick_timer_service_unsubscribe();
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}
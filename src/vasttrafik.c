#include <pebble.h>

Window* window;
TextLayer *title_layer, *line_layer, *departure_layer, *line_layer_2, *departure_layer_2;

char line_buffer[64], departure_buffer[32], line_buffer_2[64], departure_buffer_2[32];

enum
{
	KEY_LINE = 0,
  KEY_DEPARTURE = 1,
  KEY_LINE_2 = 2,
	KEY_DEPARTURE_2 = 3,
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
    case KEY_LINE:
      //Location received
      snprintf(line_buffer, sizeof("Line: couldbereallylongname"), "%s", string_value);
      text_layer_set_text(line_layer, (char*) &line_buffer);
      break;
    case KEY_DEPARTURE:
      //Temperature received
      snprintf(departure_buffer, sizeof("couldb"), "%s", string_value);
      text_layer_set_text(departure_layer, (char*) &departure_buffer);
      break;
    case KEY_LINE_2:
      //Location received
      snprintf(line_buffer_2, sizeof("Line: couldbereallylongname"), "%s", string_value);
      text_layer_set_text(line_layer_2, (char*) &line_buffer_2);
      break;
    case KEY_DEPARTURE_2:
      //Temperature received
      snprintf(departure_buffer_2, sizeof("couldb"), "%s", string_value);
      text_layer_set_text(departure_layer_2, (char*) &departure_buffer_2);
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
	title_layer = init_text_layer(GRect(0, 0, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
  text_layer_set_text(title_layer, "Vasaplatsen");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(title_layer));

  line_layer = init_text_layer(GRect(5, 30, 100, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
  text_layer_set_text(line_layer, "Line");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(line_layer));

  departure_layer = init_text_layer(GRect(-5, 30, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentRight);
  text_layer_set_text(departure_layer, "XX min");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(departure_layer));

  line_layer_2 = init_text_layer(GRect(5, 50, 100, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
	text_layer_set_text(line_layer_2, "Line");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(line_layer_2));

	departure_layer_2 = init_text_layer(GRect(-5, 50, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentRight);
	text_layer_set_text(departure_layer_2, "XX min");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(departure_layer_2));
}

void window_unload(Window *window)
{
  text_layer_destroy(title_layer);
  text_layer_destroy(line_layer);
  text_layer_destroy(departure_layer);
  text_layer_destroy(line_layer_2);
  text_layer_destroy(departure_layer_2);
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
	app_message_open(512, 512);    //Large input and output buffer sizes
  tick_timer_service_subscribe(MINUTE_UNIT, tick_callback);

  window_stack_push(window, true);
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
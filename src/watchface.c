#include <pebble.h>
#include "watchface.h"

// program
    
int main(void) {
    init();
    app_event_loop();
    cleanup();
}

static void init() {
    setlocale(LC_ALL, "");
    create_window();
    
    accel_tap_service_subscribe(handle_tap);
    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void cleanup()
{ 
    tick_timer_service_unsubscribe();
    accel_tap_service_unsubscribe();
    window_destroy(window);
}

// window

static void create_window()
{
    window = window_create();
    
#ifdef PBL_COLOR
    window_set_background_color(window, GColorYellow);
#else
    window_set_background_color(window, GColorBlack);
#endif
    
    window_set_window_handlers(window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    
    window_stack_push(window, true);
}

static void main_window_load(Window *window)
{
    create_digits();
    show_time();
}

static void main_window_unload(Window *window)
{
    for (int i = 0; i < 4; i++)
    {
        bitmap_layer_destroy(digit_layers[i]);
        gbitmap_destroy(digit_bitmaps[i]);
    }
    
#ifdef PBL_PLATFORM_BASALT
#endif
}

// time

static void create_digits()
{
    Layer* window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);

    for (int i = 0; i < 4; i++)
    {
        int xm = (i % 2) * 2 - 1;
        int ym = (i / 2) * 2 - 1;
        
        digits[i] = -1;
        digit_bitmaps[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_BLANK);
        GRect bitmap_bounds = gbitmap_get_bounds(digit_bitmaps[i]);
        digit_layers[i] = bitmap_layer_create(GRect(window_bounds.size.w / 2 - bitmap_bounds.size.w / 2 + xm * (bitmap_bounds.size.w / 2 + PADDING_X),
                                                    window_bounds.size.h / 2 - bitmap_bounds.size.h / 2 + ym * (bitmap_bounds.size.h / 2 + PADDING_Y), 
                                                    bitmap_bounds.size.w,
                                                    bitmap_bounds.size.h));

        bitmap_layer_set_compositing_mode(digit_layers[i], GCompOpSet);
        bitmap_layer_set_bitmap(digit_layers[i], digit_bitmaps[i]);
        layer_add_child(window_layer, bitmap_layer_get_layer(digit_layers[i]));
    }
}

static void show_time()
{
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    
    int hour = t->tm_hour;
    if (!clock_is_24h_style())
    {
        hour = hour % 12;
        if (hour == 0) hour = 12;
    }
    
    show_digit(0, digit_to_id(hour / 10, !clock_is_24h_style()));
    show_digit(1, digit_to_id(hour % 10, false));
    show_digit(2, digit_to_id(t->tm_min / 10, false));
    show_digit(3, digit_to_id(t->tm_min % 10, false));
}

static void show_date()
{
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    
    int date = t->tm_mday;

    show_digit(0, RESOURCE_ID_IMAGE_DIGIT_BLANK);
    show_digit(1, RESOURCE_ID_IMAGE_DIGIT_BLANK);
    show_digit(2, digit_to_id(date / 10, true));
    show_digit(3, digit_to_id(date % 10, false));
}

static void show_digit(int index, int resource_id)
{
    if (digits[index] == resource_id) return;

    bitmap_layer_set_bitmap(digit_layers[index], NULL);
    gbitmap_destroy(digit_bitmaps[index]);
    
    GBitmap* bitmap = gbitmap_create_with_resource(resource_id);
    digit_bitmaps[index] = bitmap;
    digits[index] = resource_id;
    bitmap_layer_set_bitmap(digit_layers[index], bitmap);
}

static int digit_to_id(int digit, bool use_blank_for_zero)
{
    switch (digit)
    {
        case 0: return use_blank_for_zero ? RESOURCE_ID_IMAGE_DIGIT_BLANK
                                          : RESOURCE_ID_IMAGE_DIGIT0;
        case 1: return RESOURCE_ID_IMAGE_DIGIT1;
        case 2: return RESOURCE_ID_IMAGE_DIGIT2;
        case 3: return RESOURCE_ID_IMAGE_DIGIT3;
        case 4: return RESOURCE_ID_IMAGE_DIGIT4;
        case 5: return RESOURCE_ID_IMAGE_DIGIT5;
        case 6: return RESOURCE_ID_IMAGE_DIGIT6;
        case 7: return RESOURCE_ID_IMAGE_DIGIT7;
        case 8: return RESOURCE_ID_IMAGE_DIGIT8;
        case 9: return RESOURCE_ID_IMAGE_DIGIT9;
        default: return RESOURCE_ID_IMAGE_DIGIT_BLANK;
    }
}

// events

static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) 
{
    if (mode == MODE_TIME) show_time();
    if (mode == MODE_DATE) show_date();
}

static void handle_tap(AccelAxisType axis, int32_t direction)
{
    return; // TODO temporarily disabled because people may flick to light

    if (mode == MODE_TIME)
    {
        mode = MODE_DATE;
        
        show_date();
        
        reset_timer = app_timer_register(3000, handle_mode_reset, NULL);
    }
}

static void handle_mode_reset(void* data)
{
    mode = MODE_TIME;
    
    show_time();
}
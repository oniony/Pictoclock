#include <pebble.h>

// constants
    
const int MODE_TIME = 0;
const int MODE_DATE = 1;
const int PADDING_X = 0;
const int PADDING_Y = 4;

// variables
    
static Window* window;
static int digits[4];
static BitmapLayer* digit_layers[4];
static GBitmap* digit_bitmaps[4];
static int mode = 0;
static AppTimer* reset_timer;

// program
    
int main(void);
static void init();
static void cleanup();

// window
    
static void create_window();
static void main_window_load(Window *window);
static void main_window_unload(Window *window);

// rendering

static void create_digits();
static void show_time();
static void show_date();
static int digit_to_id(int digit, bool use_blank_for_zero);
static void show_digit(int index, int resource_id);

// events

static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed);
static void handle_tap(AccelAxisType axis, int32_t direction); 
static void handle_mode_reset(void* data);
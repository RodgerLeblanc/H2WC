#include <pebble.h>
/*
* User settings
* Show Clock
* Show Weather
* Autohide
*/

// Config
#define QTP_WINDOW_TIMEOUT 4000
#define QTP_K_SHOW_TIME 1
#define QTP_K_SHOW_WEATHER 2
#define QTP_K_AUTOHIDE 4
#define QTP_K_DEGREES_F 8
#define QTP_K_INVERT 16

#define QTP_SCREEN_WIDTH        144
#define QTP_SCREEN_HEIGHT       168

#define QTP_PADDING_Y 5
#define QTP_PADDING_X 5
#define QTP_BT_ICON_SIZE 32
#define QTP_BAT_ICON_SIZE 32
#define QTP_WEATHER_SIZE 32
#define QTP_TIME_HEIGHT 32
#define QTP_BATTERY_BASE_Y 0
#define QTP_BLUETOOTH_BASE_Y QTP_BAT_ICON_SIZE + 5
#define QTP_WEATHER_BASE_Y QTP_BAT_ICON_SIZE + QTP_BT_ICON_SIZE + 10



// Items
static Window *qtp_window;
bool qtp_is_showing;
TextLayer *qtp_battery_text_layer;
TextLayer *qtp_phone_battery_text_layer;
TextLayer *qtp_calendar_text_layer;
AppTimer *qtp_hide_timer;
int qtp_conf;
int qtp_phone_percent;
char qtp_calendar_events[200];

// Methods
void qtp_setup();
void qtp_app_deinit();

void qtp_show();
void qtp_hide();
void qtp_timeout();

void qtp_tap_handler(AccelAxisType axis, int32_t direction);
void qtp_click_config_provider(Window *window);
void qtp_back_click_responder(ClickRecognizerRef recognizer, void *context);

void qtp_update_battery_status(bool mark_dirty);
void qtp_update_phone_battery_status();
void qtp_update_calendar_status();

void qtp_init();
void qtp_deinit();

// Helpers
bool qtp_is_autohide();
bool qtp_is_invert();
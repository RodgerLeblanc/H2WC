#include "pebble.h"
#include "QTPlus.h"

#define WATCHFACE_VERSION "1.1"
	
#define TIME_FRAME     (GRect(0, -5, 144, 60))
#define DATE_FRAME     (GRect(0, 55, 144, 90-55))
#define HUB_FRAME      (GRect(0, 90, 144, 168-90))
#define HUB_BOUNDS      (GRect(0, 90, 144*2, (168-90) + 36))

#define WIDTH 			(144)
#define HEIGHT_OFFSET 	(78)

/* Persistent storage keys */
#define CONFIG_KEY_BATTERY   (0)
#define USER_COLORS (1)
	
	
/* Global variables to keep track of the UI elements */
static Window *window;
static TextLayer *date_layer;
static TextLayer *time_layer;
#ifdef PBL_PLATFORM_APLITE
	InverterLayer *inv_layer;
#endif
	
static char date_text[] = "XXX 00";
static char time_text[] = "00:00";

/* Preload the fonts */
static GFont large_font, font_time, font_date;

static Layer *hub_layer;

static TextLayer *template0_value_layer;
static char template0_value[16];
static BitmapLayer *template0_icon_layer;
static GBitmap *template0_icon_bitmap = NULL;
static int template0_icon_number = -1;

static TextLayer *template1_value_layer;
static char template1_value[16];
static BitmapLayer *template1_icon_layer;
static GBitmap *template1_icon_bitmap = NULL;
static int template1_icon_number = -1;

static TextLayer *template2_value_layer;
static char template2_value[16];
static BitmapLayer *template2_icon_layer;
static GBitmap *template2_icon_bitmap = NULL;
static int template2_icon_number = -1;

static TextLayer *template3_value_layer;
static char template3_value[16];
static BitmapLayer *template3_icon_layer;
static GBitmap *template3_icon_bitmap = NULL;
static int template3_icon_number = -1;

static TextLayer *template4_value_layer;
static char template4_value[16];
static BitmapLayer *template4_icon_layer;
static GBitmap *template4_icon_bitmap = NULL;
static int template4_icon_number = -1;

static TextLayer *template5_value_layer;
static char template5_value[16];
static BitmapLayer *template5_icon_layer;
static GBitmap *template5_icon_bitmap = NULL;
static int template5_icon_number = -1;

static TextLayer *template6_value_layer;
static char template6_value[16];
static BitmapLayer *template6_icon_layer;
static GBitmap *template6_icon_bitmap = NULL;
static int template6_icon_number = -1;

static GColor top_color, bottom_color, time_color;

static PropertyAnimation *prop_anim;
AppTimer *tap_timer;

enum Watchface {
  WATCHFACE_1, 
  WATCHFACE_4,
  WATCHFACE_6,
};

int watchface_used = WATCHFACE_1;

enum HubKey {
  HUB1_ACCOUNT_KEY = 0x00,  		// TUPLE_INT
  HUB1_VALUE_KEY = 0x01,			// TUPLE_CSTRING
  HUB2_ACCOUNT_KEY = 0x02,       // TUPLE_INT
  HUB2_VALUE_KEY = 0x03,  		// TUPLE_CSTRING
  HUB3_ACCOUNT_KEY = 0x04,       // TUPLE_INT
  HUB3_VALUE_KEY = 0x05,  		// TUPLE_CSTRING
  HUB4_ACCOUNT_KEY = 0x06,       // TUPLE_INT
  HUB4_VALUE_KEY = 0x07, 		// TUPLE_CSTRING
  HUB5_ACCOUNT_KEY = 0x08,       // TUPLE_INT
  HUB5_VALUE_KEY = 0x09,  		// TUPLE_CSTRING
  HUB6_ACCOUNT_KEY = 0x0A,       // TUPLE_INT
  HUB6_VALUE_KEY = 0x0B,  		// TUPLE_CSTRING
  VIBRATE_KEY = 0x0C,  			// TUPLE_INT
  PHONE_PERCENT = 0x0D,  		// TUPLE_INT
  CALENDAR_EVENT_NAME = 0x0E,	// TUPLE_CSTRING
  SLOT_COUNTER_KEY = 0x0F,		// TUPLE_INT
  SHOW_CALENDAR_SCREEN_KEY = 0x10,
  TOP_KEY_COLOR_RED = 0x11,
  TOP_KEY_COLOR_GREEN = 0x12,
  TOP_KEY_COLOR_BLUE = 0x13,
  BOTTOM_KEY_COLOR_RED = 0x14,
  BOTTOM_KEY_COLOR_GREEN = 0x15,
  BOTTOM_KEY_COLOR_BLUE = 0x16,
  TIME_KEY_COLOR_RED = 0x17,
  TIME_KEY_COLOR_GREEN = 0x18,
  TIME_KEY_COLOR_BLUE = 0x19
};

static uint32_t ALL_ICONS_58[] = {
    RESOURCE_ID_CLEAR_DAY_58,				// 0
	RESOURCE_ID_CLEAR_NIGHT_58,				// 1
	RESOURCE_ID_CLOUD_ERROR_58,				// 2
	RESOURCE_ID_CLOUDY_58,					// 3
	RESOURCE_ID_COLD_58,					// 4
	RESOURCE_ID_DRIZZLE_58,					// 5
	RESOURCE_ID_FOG_58,						// 6
	RESOURCE_ID_HOT_58,						// 7
	RESOURCE_ID_NOT_AVAILABLE_58,			// 8
	RESOURCE_ID_PARTLY_CLOUDY_DAY_58,		// 9
	RESOURCE_ID_PARTLY_CLOUDY_NIGHT_58,  	// 10
	RESOURCE_ID_PHONE_ERROR_58,				// 11
	RESOURCE_ID_RAIN_58,					// 12
	RESOURCE_ID_RAIN_SLEET_58,				// 13
	RESOURCE_ID_RAIN_SNOW_58,				// 14
	RESOURCE_ID_SLEET_58,					// 15
	RESOURCE_ID_SNOW_58,					// 16
	RESOURCE_ID_SNOW_SLEET_58,				// 17
	RESOURCE_ID_THUNDER_58,					// 18
	RESOURCE_ID_WIND_58,					// 19
	RESOURCE_ID_ICON_BBM_58,  				// 20
  	RESOURCE_ID_ICON_CALL_58,				// 21
  	RESOURCE_ID_ICON_EMAIL_58,				// 22
  	RESOURCE_ID_ICON_FACEBOOK_58,			// 23
  	RESOURCE_ID_ICON_LINKEDIN_58,			// 24
  	RESOURCE_ID_ICON_NOTIFICATION_58,		// 25
  	RESOURCE_ID_ICON_PIN_58,				// 26
  	RESOURCE_ID_ICON_TEXT_58,				// 27
  	RESOURCE_ID_ICON_TWITTER_58,			// 28
  	RESOURCE_ID_ICON_VOICEMAIL_58,			// 29
  	RESOURCE_ID_ICON_WHATSAPP_58,			// 30
  	RESOURCE_ID_ICON_EMPTY_58,				// 31
  	RESOURCE_ID_ICON_BLAQ_58,				// 32
  	RESOURCE_ID_ICON_SLACK_58,				// 33
  	RESOURCE_ID_ICON_INSTAGRAM_58,			// 34
  	RESOURCE_ID_ICON_QUICKPOST_58,			// 35
  	RESOURCE_ID_ICON_TWITTLY_58,			// 36
  	RESOURCE_ID_ICON_WECHAT_58,				// 37
  	RESOURCE_ID_ICON_HG10_58,				// 38
  	RESOURCE_ID_ICON_GOOGLE_TALK_58,		// 39
  	RESOURCE_ID_ICON_CALENDAR_58,			// 40
  	RESOURCE_ID_ICON_VIP_58,				// 41
  	RESOURCE_ID_ICON_FOURSQUARE_58			// 42
};

static uint32_t ALL_ICONS_34[] = {
    RESOURCE_ID_CLEAR_DAY_34,				// 0
	RESOURCE_ID_CLEAR_NIGHT_34,				// 1
	RESOURCE_ID_CLOUD_ERROR_34,				// 2
	RESOURCE_ID_CLOUDY_34,					// 3
	RESOURCE_ID_COLD_34,					// 4
	RESOURCE_ID_DRIZZLE_34,					// 5
	RESOURCE_ID_FOG_34,						// 6
	RESOURCE_ID_HOT_34,						// 7
	RESOURCE_ID_NOT_AVAILABLE_34,			// 8
	RESOURCE_ID_PARTLY_CLOUDY_DAY_34,		// 9
	RESOURCE_ID_PARTLY_CLOUDY_NIGHT_34,  	// 10
	RESOURCE_ID_PHONE_ERROR_34,				// 11
	RESOURCE_ID_RAIN_34,					// 12
	RESOURCE_ID_RAIN_SLEET_34,				// 13
	RESOURCE_ID_RAIN_SNOW_34,				// 14
	RESOURCE_ID_SLEET_34,					// 15
	RESOURCE_ID_SNOW_34,					// 16
	RESOURCE_ID_SNOW_SLEET_34,				// 17
	RESOURCE_ID_THUNDER_34,					// 18
	RESOURCE_ID_WIND_34,					// 19
	RESOURCE_ID_ICON_BBM_34,  				// 20
  	RESOURCE_ID_ICON_CALL_34,				// 21
  	RESOURCE_ID_ICON_EMAIL_34,				// 22
  	RESOURCE_ID_ICON_FACEBOOK_34,			// 23
  	RESOURCE_ID_ICON_LINKEDIN_34,			// 24
  	RESOURCE_ID_ICON_NOTIFICATION_34,		// 25
  	RESOURCE_ID_ICON_PIN_34,				// 26
  	RESOURCE_ID_ICON_TEXT_34,				// 27
  	RESOURCE_ID_ICON_TWITTER_34,			// 28
  	RESOURCE_ID_ICON_VOICEMAIL_34,			// 29
  	RESOURCE_ID_ICON_WHATSAPP_34,			// 30
  	RESOURCE_ID_ICON_EMPTY_34,				// 31
  	RESOURCE_ID_ICON_BLAQ_34,				// 32
  	RESOURCE_ID_ICON_SLACK_34,				// 33
  	RESOURCE_ID_ICON_INSTAGRAM_34,			// 34
  	RESOURCE_ID_ICON_QUICKPOST_34,			// 35
  	RESOURCE_ID_ICON_TWITTLY_34,			// 36
  	RESOURCE_ID_ICON_WECHAT_34,				// 37
  	RESOURCE_ID_ICON_HG10_34,				// 38
  	RESOURCE_ID_ICON_GOOGLE_TALK_34,		// 39
  	RESOURCE_ID_ICON_CALENDAR_34,			// 40
  	RESOURCE_ID_ICON_VIP_34,				// 41
  	RESOURCE_ID_ICON_FOURSQUARE_34			// 42
};

int valueRead, valueWritten;

typedef struct saved_colors {
	int top_color_red;
	int top_color_green;
	int top_color_blue;
	int bottom_color_red;
	int bottom_color_green;
	int bottom_color_blue;
	int time_color_red;
	int time_color_green;
	int time_color_blue;
} __attribute__((__packed__)) saved_colors;

static saved_colors user_colors = {
	// Default: Top=Black, Bottom=Red, Time=LightBlue
	.top_color_red = 0,
	.top_color_green = 0,
	.top_color_blue = 0,
	.bottom_color_red = 255,
	.bottom_color_green = 0,
	.bottom_color_blue = 0,
	.time_color_red = 170,
	.time_color_green = 255,
	.time_color_blue = 255	
};

void loadPersistentSettings() {
	if (persist_exists(USER_COLORS))
		valueRead = persist_read_data(USER_COLORS, &user_colors, sizeof(user_colors));
}

void savePersistentSettings() {
	valueWritten = persist_write_data(USER_COLORS, &user_colors, sizeof(user_colors));
}

static const VibePattern custom_pattern_1 = {
  .durations = (uint32_t []) {100},
  .num_segments = 1
};  // 1 short vibes

static const VibePattern custom_pattern_2 = {
  .durations = (uint32_t []) {100, 100, 100},
  .num_segments = 3
};  // 2 short vibes

static const VibePattern custom_pattern_3 = {
  .durations = (uint32_t []) {100, 100, 100, 100, 100},
  .num_segments = 5
};  // 3 short vibes

static void ask_hub2watch_to_refresh() {
	// Sends a dumb appMessage to Hub2Watch to force it to refresh
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	int dumbVariable = 111;
	dict_write_int(iter, 1, &dumbVariable, sizeof(dumbVariable), true);
	app_message_outbox_send();
}

void battery_handler(BatteryChargeState charge) {
	// Sends an appMessage to Hub2Watch with the new Battery state
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	int percent = charge.charge_percent;
	dict_write_int(iter, 0, &percent, sizeof(int), true);
	app_message_outbox_send();
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
  if (units_changed & MINUTE_UNIT) {
    // Update the time - Fix to deal with 12 / 24 centering bug
    time_t currentTime = time(0);
    struct tm *currentLocalTime = localtime(&currentTime);

    // Manually format the time as 12 / 24 hour, as specified
    strftime(   time_text, 
                sizeof(time_text), 
                clock_is_24h_style() ? "%R" : "%I:%M", 
                currentLocalTime);

    // Drop the first char of time_text if needed
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
      memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }

    text_layer_set_text(time_layer, time_text);
  }
  if (units_changed & DAY_UNIT) {
	// Percents are currently shown, we don't want to override it yet
	if (tap_timer)
		return;
	  
    // Update the date - Without a leading 0 on the day of the month
    char day_text[4];
    strftime(day_text, sizeof(day_text), "%a", tick_time);
    snprintf(date_text, sizeof(date_text), "%s %i", day_text, tick_time->tm_mday);
    text_layer_set_text(date_layer, date_text);
  }
}

void make_vibes_1()  {
	vibes_enqueue_custom_pattern(custom_pattern_1);
}

void make_vibes_2()  {
	vibes_enqueue_custom_pattern(custom_pattern_2);
}

void make_vibes_3()  {
	vibes_enqueue_custom_pattern(custom_pattern_3);
}

void tap_timeout() {
	tap_timer = NULL;
	
	// Put back normal value to Date text layer
	time_t now = time(NULL);
	handle_tick(localtime(&now), DAY_UNIT);
}

void set_color() {
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Top colors: %d %d %d", user_colors.top_color_red, user_colors.top_color_green, user_colors.top_color_blue);
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Bottom colors: %d %d %d", user_colors.bottom_color_red, user_colors.bottom_color_green, user_colors.bottom_color_blue);
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Time colors: %d %d %d", user_colors.time_color_red, user_colors.time_color_green, user_colors.time_color_blue);
	
  	if ((user_colors.top_color_red >= 0) && (user_colors.top_color_green >= 0) && (user_colors.top_color_blue >= 0)) {
    	// Apply the color if available
		#ifdef PBL_PLATFORM_APLITE
			if (window)
    			window_set_background_color(window, GColorBlack);
		#else
    		top_color = GColorFromRGB(user_colors.top_color_red, user_colors.top_color_green, user_colors.top_color_blue);
			if (window)
    			window_set_background_color(window, top_color);
		#endif
	}

  	if ((user_colors.bottom_color_red >= 0) && (user_colors.bottom_color_green >= 0) && (user_colors.bottom_color_blue >= 0)) {
		#ifdef PBL_PLATFORM_APLITE
			bottom_color = GColorWhite;
		#else
    		bottom_color = GColorFromRGB(user_colors.bottom_color_red, user_colors.bottom_color_green, user_colors.bottom_color_blue);
		#endif
		if (hub_layer)
			layer_mark_dirty(hub_layer);
  	}
	
  	if ((user_colors.time_color_red >= 0) && (user_colors.time_color_green >= 0) && (user_colors.time_color_blue >= 0)) {
    	// Apply the color if available
		#ifdef PBL_PLATFORM_APLITE
			if (time_layer)
    			text_layer_set_text_color(time_layer, GColorWhite);
			if (date_layer) 
    			text_layer_set_text_color(date_layer, GColorWhite);
		#else
    		time_color = GColorFromRGB(user_colors.time_color_red, user_colors.time_color_green, user_colors.time_color_blue);
			if (time_layer)
    			text_layer_set_text_color(time_layer, time_color);
			if (date_layer) {
    			text_layer_set_text_color(date_layer, time_color);
			}
		#endif
  	}
	
	savePersistentSettings();
}

/* Handle taps from the hardware */
void tap_handler(AccelAxisType axis, int32_t direction) {
	BatteryChargeState charge_state = battery_state_service_peek();
    static char battery_text[] = "100%  100%";
    snprintf(battery_text, sizeof(battery_text), "%d%%  %d%%", charge_state.charge_percent, qtp_phone_percent);
    text_layer_set_text(date_layer, battery_text);
	
	if (tap_timer)
		app_timer_cancel(tap_timer);
	
	tap_timer = app_timer_register(5000, tap_timeout, NULL);
}

void update_configuration(void)
{
    bool show_battery = 1;    /* default to true */

    if (persist_exists(CONFIG_KEY_BATTERY))
    {
        show_battery = persist_read_bool(CONFIG_KEY_BATTERY);
    }
	
	if (show_battery) {
		accel_tap_service_unsubscribe();
		qtp_setup();
	} else {
		qtp_app_deinit();
        accel_tap_service_subscribe(&tap_handler);
	}
}

static void update_proc(Layer *layer, GContext *ctx) {
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "update_proc called");
	const GRect bounds = layer_get_bounds(layer);
    graphics_context_set_fill_color(ctx, bottom_color);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
};

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
#ifdef PBL_SDK_2
  property_animation_destroy(prop_anim);
#endif
}

static void animate_hub_layer(GRect start, GRect finish) {
  prop_anim = property_animation_create_layer_frame(hub_layer, &start, &finish);
  animation_set_duration((Animation*)prop_anim, 1000);
  animation_set_handlers((Animation*)prop_anim, (AnimationHandlers) {
    .stopped = anim_stopped_handler
  }, NULL);
  animation_schedule((Animation*)prop_anim);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Used for refreshing screen if new colors have been received
  bool refresh_needed = false;
	
  Tuple *tuple = dict_read_first(iter);
  while (tuple) {
  	switch (tuple->key) {
    	case HUB1_ACCOUNT_KEY: {
			int tupleValue = tuple->value->uint8;
			if (template0_icon_number != tupleValue) {
				if (template0_icon_bitmap) {
        			gbitmap_destroy(template0_icon_bitmap);
            	}
		
            	template0_icon_bitmap = gbitmap_create_with_resource(ALL_ICONS_58[tupleValue]);
            	bitmap_layer_set_bitmap(template0_icon_layer, template0_icon_bitmap);
				
				template0_icon_number = tupleValue;
			}

			if (template1_icon_number != tupleValue) {
				if (template1_icon_bitmap) {
        			gbitmap_destroy(template1_icon_bitmap);
				}
		
            	template1_icon_bitmap = gbitmap_create_with_resource(ALL_ICONS_34[tupleValue]);
           		bitmap_layer_set_bitmap(template1_icon_layer, template1_icon_bitmap);
				
				template1_icon_number = tupleValue;
			}
      		break;
		}
    	case HUB1_VALUE_KEY: {
			text_layer_set_text(template0_value_layer, tuple->value->cstring);
			text_layer_set_text(template1_value_layer, tuple->value->cstring);
      		break;
		}
    	case HUB2_ACCOUNT_KEY: {
			int tupleValue = tuple->value->uint8;
			if (template2_icon_number != tupleValue) {
				if (template2_icon_bitmap) {
        			gbitmap_destroy(template2_icon_bitmap);
        		}
	
        		template2_icon_bitmap = gbitmap_create_with_resource(ALL_ICONS_34[tupleValue]);
        		bitmap_layer_set_bitmap(template2_icon_layer, template2_icon_bitmap);
				template2_icon_number = tupleValue;
			}
      		break;
		}
    	case HUB2_VALUE_KEY: {
			text_layer_set_text(template2_value_layer, tuple->value->cstring);
      		break;
		}
    	case HUB3_ACCOUNT_KEY: {
			int tupleValue = tuple->value->uint8;
			if (template3_icon_number != tupleValue) {
				if (template3_icon_bitmap) {
        			gbitmap_destroy(template3_icon_bitmap);
        		}
	
        		template3_icon_bitmap = gbitmap_create_with_resource(ALL_ICONS_34[tupleValue]);
        		bitmap_layer_set_bitmap(template3_icon_layer, template3_icon_bitmap);
				template3_icon_number = tupleValue;
			}
      		break;
		}
    	case HUB3_VALUE_KEY: {
			text_layer_set_text(template3_value_layer, tuple->value->cstring);
      		break;
		}
    	case HUB4_ACCOUNT_KEY: {
			int tupleValue = tuple->value->uint8;
			if (template4_icon_number != tupleValue) {
				if (template4_icon_bitmap) {
        			gbitmap_destroy(template4_icon_bitmap);
        		}
	
        		template4_icon_bitmap = gbitmap_create_with_resource(ALL_ICONS_34[tupleValue]);
        		bitmap_layer_set_bitmap(template4_icon_layer, template4_icon_bitmap);
				template4_icon_number = tupleValue;
			}
      		break;
		}
    	case HUB4_VALUE_KEY: {
			text_layer_set_text(template4_value_layer, tuple->value->cstring);
      		break;
		}
    	case HUB5_ACCOUNT_KEY: {
			int tupleValue = tuple->value->uint8;
			if (template5_icon_number != tupleValue) {
				if (template5_icon_bitmap) {
        			gbitmap_destroy(template5_icon_bitmap);
        		}
	
        		template5_icon_bitmap = gbitmap_create_with_resource(ALL_ICONS_34[tupleValue]);
        		bitmap_layer_set_bitmap(template5_icon_layer, template5_icon_bitmap);
				template5_icon_number = tupleValue;
			}
      		break;
		}
    	case HUB5_VALUE_KEY: {
			text_layer_set_text(template5_value_layer, tuple->value->cstring);
      		break;
		}
    	case HUB6_ACCOUNT_KEY: {
			int tupleValue = tuple->value->uint8;
			if (template6_icon_number != tupleValue) {
				if (template6_icon_bitmap) {
        			gbitmap_destroy(template6_icon_bitmap);
        		}
	
        		template6_icon_bitmap = gbitmap_create_with_resource(ALL_ICONS_34[tupleValue]);
        		bitmap_layer_set_bitmap(template6_icon_layer, template6_icon_bitmap);
				template6_icon_number = tupleValue;
			}
      		break;
		}
    	case HUB6_VALUE_KEY: {
			text_layer_set_text(template6_value_layer, tuple->value->cstring);
      		break;
		}
    	case VIBRATE_KEY: {
			int tupleValue = tuple->value->uint8;
	  		if (tupleValue == 1) {
				make_vibes_1();
	  		}
	  		else if (tupleValue == 2) {
				make_vibes_2();
	  		}
	  		else if (tupleValue == 3) {
				make_vibes_3();
	  		}
	  		else if (tupleValue > 3) {
				vibes_long_pulse();
	  		}
      		break;
		}
    	case PHONE_PERCENT: {
			qtp_phone_percent = tuple->value->uint8;
      		break;
		}
    	case CALENDAR_EVENT_NAME: {
			strncpy(qtp_calendar_events, tuple->value->cstring, 100);
			ask_hub2watch_to_refresh();
      		break;
		}
    	case SLOT_COUNTER_KEY: {
			int this_watchface;
			switch (tuple->value->uint8) {
				case 0: {
					this_watchface = WATCHFACE_1;
					break;
				}
				case 1:
				case 2:
				case 3: {
					this_watchface = WATCHFACE_4;
					break;
				}
				case 4:
				case 5:
				case 6: {
					this_watchface = WATCHFACE_6;
					break;
				}
				default: {
					this_watchface = WATCHFACE_1;
					break;
				}
			}
		
			if (this_watchface != watchface_used) {
				GRect start = HUB_BOUNDS;
				GRect finish = HUB_BOUNDS;
			
				switch (watchface_used) {
					case WATCHFACE_1: {
						break;
					}
					case WATCHFACE_4: {
						start.origin.x = start.origin.x - 144;
						break;
					}
					case WATCHFACE_6: {
						start.origin.x = start.origin.x - 144;
						start.origin.y = start.origin.y - 36;
						break;
					}
				}
			
				switch (this_watchface) {
					case WATCHFACE_1: {
						break;
					}
					case WATCHFACE_4: {
						finish.origin.x = finish.origin.x - 144;
						break;
					}
					case WATCHFACE_6: {
						finish.origin.x = finish.origin.x - 144;
						finish.origin.y = finish.origin.y - 36;
						break;
					}
				}

				animate_hub_layer(start, finish);
				watchface_used = this_watchface;
			}
      		break;
		}
    	case SHOW_CALENDAR_SCREEN_KEY: {
			int tupleValue = tuple->value->uint8;
			if (tupleValue == 0) {
    			persist_write_bool(CONFIG_KEY_BATTERY, false);
				update_configuration();
			}
			else {
				if (tupleValue == 1) {
    				persist_write_bool(CONFIG_KEY_BATTERY, true);
		    		update_configuration();
				}
  			}
      		break;
		}
    	case TOP_KEY_COLOR_RED: {
			user_colors.top_color_red = tuple->value->int32;
			refresh_needed = true;
			break;
		}
    	case TOP_KEY_COLOR_GREEN: {
			user_colors.top_color_green = tuple->value->int32;
			refresh_needed = true;
      		break;
		}
    	case TOP_KEY_COLOR_BLUE: {
			user_colors.top_color_blue = tuple->value->int32;
			refresh_needed = true;
      		break;
		}
    	case BOTTOM_KEY_COLOR_RED: {
			user_colors.bottom_color_red = tuple->value->int32;
			refresh_needed = true;
      		break;
		}
    	case BOTTOM_KEY_COLOR_GREEN: {
			user_colors.bottom_color_green = tuple->value->int32;
			refresh_needed = true;
      		break;
		}
    	case BOTTOM_KEY_COLOR_BLUE: {
			user_colors.bottom_color_blue = tuple->value->int32;
			refresh_needed = true;
      		break;
		}
    	case TIME_KEY_COLOR_RED: {
			user_colors.time_color_red = tuple->value->int32;
			refresh_needed = true;
      		break;
		}
    	case TIME_KEY_COLOR_GREEN: {
			user_colors.time_color_green = tuple->value->int32;
			refresh_needed = true;
      		break;
		}
    	case TIME_KEY_COLOR_BLUE: {
			user_colors.time_color_blue = tuple->value->int32;
			refresh_needed = true;
      		break;
		}
  	}
  	tuple = dict_read_next(iter);
  }
	
  if (refresh_needed) {
	// Apply the color if available
	#ifdef PBL_PLATFORM_APLITE
		window_set_background_color(window, GColorBlack);
		bottom_color = GColorWhite;
    	text_layer_set_text_color(time_layer, GColorWhite);
    	text_layer_set_text_color(date_layer, GColorWhite);
	#else 
    	top_color = GColorFromRGB(user_colors.top_color_red, user_colors.top_color_green, user_colors.top_color_blue);
    	bottom_color = GColorFromRGB(user_colors.bottom_color_red, user_colors.bottom_color_green, user_colors.bottom_color_blue);
    	time_color = GColorFromRGB(user_colors.time_color_red, user_colors.time_color_green, user_colors.time_color_blue);
		window_set_background_color(window, top_color);
		layer_mark_dirty(hub_layer);
		text_layer_set_text_color(time_layer, time_color);
		text_layer_set_text_color(date_layer, time_color);
	#endif
	  
	// Save new colors to persistent storage
	savePersistentSettings();
	
	// Refresh watchface
	ask_hub2watch_to_refresh();
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  top_color = GColorBlack;
	
  #ifdef PBL_PLATFORM_APLITE
  	bottom_color = GColorWhite;
  #else
  	bottom_color = GColorRed;
  #endif
	
  time_color = GColorWhite;
	
  if (persist_exists(USER_COLORS))
  	  loadPersistentSettings();
  else	
	  savePersistentSettings();
	
  set_color();
	
  window_set_background_color(window, top_color);
	
  time_layer = text_layer_create(TIME_FRAME);
  text_layer_set_text_color(time_layer, time_color);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, font_time);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  date_layer = text_layer_create(DATE_FRAME);
  text_layer_set_text_color(date_layer, time_color);
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_font(date_layer, font_date);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  hub_layer = layer_create(HUB_BOUNDS);
  layer_set_update_proc(hub_layer, update_proc);
	
  template0_icon_layer = bitmap_layer_create(GRect(0+9, 9, 60, 60));
  #ifdef PBL_SDK_3
	// Allow transparent images
	bitmap_layer_set_compositing_mode(template0_icon_layer, GCompOpSet);
  #endif  
  layer_add_child(hub_layer, bitmap_layer_get_layer(template0_icon_layer));

  template0_value_layer = text_layer_create(GRect(69, 17, 150-69, 60));
  text_layer_set_text_color(template0_value_layer, GColorBlack);
  text_layer_set_background_color(template0_value_layer, GColorClear);
  text_layer_set_font(template0_value_layer, large_font);
  text_layer_set_text_alignment(template0_value_layer, GTextAlignmentCenter);
  text_layer_set_text(template0_value_layer, template0_value);
  layer_add_child(hub_layer, text_layer_get_layer(template0_value_layer));

  template1_icon_layer = bitmap_layer_create(GRect(WIDTH+0, 3, 34, 34));
  #ifdef PBL_SDK_3
	// Allow transparent images
	bitmap_layer_set_compositing_mode(template1_icon_layer, GCompOpSet);
  #endif  
  layer_add_child(hub_layer, bitmap_layer_get_layer(template1_icon_layer));

  template1_value_layer = text_layer_create(GRect(WIDTH+34, 3, 72-34, 39-3));
  text_layer_set_text_color(template1_value_layer, GColorBlack);
  text_layer_set_background_color(template1_value_layer, GColorClear);
  text_layer_set_font(template1_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(template1_value_layer, GTextAlignmentCenter);
  text_layer_set_text(template1_value_layer, template1_value);
  layer_add_child(hub_layer, text_layer_get_layer(template1_value_layer));

  template2_icon_layer = bitmap_layer_create(GRect(WIDTH+72, 3, 34, 34));
  #ifdef PBL_SDK_3
	// Allow transparent images
	bitmap_layer_set_compositing_mode(template2_icon_layer, GCompOpSet);
  #endif  
  layer_add_child(hub_layer, bitmap_layer_get_layer(template2_icon_layer));

  template2_value_layer = text_layer_create(GRect(WIDTH+72+34, 3, 72-34, 39-3));
  text_layer_set_text_color(template2_value_layer, GColorBlack);
  text_layer_set_background_color(template2_value_layer, GColorClear);
  text_layer_set_font(template2_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(template2_value_layer, GTextAlignmentCenter);
  text_layer_set_text(template2_value_layer, template2_value);
  layer_add_child(hub_layer, text_layer_get_layer(template2_value_layer));

  template3_icon_layer = bitmap_layer_create(GRect(WIDTH+0, 39+3, 34, 34));
  #ifdef PBL_SDK_3
	// Allow transparent images
	bitmap_layer_set_compositing_mode(template3_icon_layer, GCompOpSet);
  #endif  
  layer_add_child(hub_layer, bitmap_layer_get_layer(template3_icon_layer));

  template3_value_layer = text_layer_create(GRect(WIDTH+34, 39+3, 72-34, 39-3));
  text_layer_set_text_color(template3_value_layer, GColorBlack);
  text_layer_set_background_color(template3_value_layer, GColorClear);
  text_layer_set_font(template3_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(template3_value_layer, GTextAlignmentCenter);
  text_layer_set_text(template3_value_layer, template3_value);
  layer_add_child(hub_layer, text_layer_get_layer(template3_value_layer));

  template4_icon_layer = bitmap_layer_create(GRect(WIDTH+72, 39+3, 34, 34));
  #ifdef PBL_SDK_3
	// Allow transparent images
	bitmap_layer_set_compositing_mode(template4_icon_layer, GCompOpSet);
  #endif  
  layer_add_child(hub_layer, bitmap_layer_get_layer(template4_icon_layer));

  template4_value_layer = text_layer_create(GRect(WIDTH+72+34, 39+3, 72-34, 39-3));
  text_layer_set_text_color(template4_value_layer, GColorBlack);
  text_layer_set_background_color(template4_value_layer, GColorClear);
  text_layer_set_font(template4_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(template4_value_layer, GTextAlignmentCenter);
  text_layer_set_text(template4_value_layer, template4_value);
  layer_add_child(hub_layer, text_layer_get_layer(template4_value_layer));

  template5_icon_layer = bitmap_layer_create(GRect(WIDTH+0, HEIGHT_OFFSET+3, 34, 34));
  #ifdef PBL_SDK_3
	// Allow transparent images
	bitmap_layer_set_compositing_mode(template5_icon_layer, GCompOpSet);
  #endif  
  layer_add_child(hub_layer, bitmap_layer_get_layer(template5_icon_layer));

  template5_value_layer = text_layer_create(GRect(WIDTH+34, HEIGHT_OFFSET+3, 72-34, 39-3));
  text_layer_set_text_color(template5_value_layer, GColorBlack);
  text_layer_set_background_color(template5_value_layer, GColorClear);
  text_layer_set_font(template5_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(template5_value_layer, GTextAlignmentCenter);
  text_layer_set_text(template5_value_layer, template5_value);
  layer_add_child(hub_layer, text_layer_get_layer(template5_value_layer));

  template6_icon_layer = bitmap_layer_create(GRect(WIDTH+72, HEIGHT_OFFSET+3, 34, 34));
  #ifdef PBL_SDK_3
	// Allow transparent images
	bitmap_layer_set_compositing_mode(template6_icon_layer, GCompOpSet);
  #endif  
  layer_add_child(hub_layer, bitmap_layer_get_layer(template6_icon_layer));

  template6_value_layer = text_layer_create(GRect(WIDTH+72+34, HEIGHT_OFFSET+3, 72-34, 39-3));
  text_layer_set_text_color(template6_value_layer, GColorBlack);
  text_layer_set_background_color(template6_value_layer, GColorClear);
  text_layer_set_font(template6_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(template6_value_layer, GTextAlignmentCenter);
  text_layer_set_text(template6_value_layer, template6_value);
  layer_add_child(hub_layer, text_layer_get_layer(template6_value_layer));

  layer_add_child(window_layer, hub_layer);
  layer_mark_dirty(hub_layer);
}

static void window_unload(Window *window) {
  #ifdef PBL_PLATFORM_APLITE
	if (inv_layer)
		inverter_layer_destroy(inv_layer);
  #endif	
	
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);

  if (template0_icon_bitmap) {
    gbitmap_destroy(template0_icon_bitmap);
  }
  text_layer_destroy(template0_value_layer);
  bitmap_layer_destroy(template0_icon_layer);

  if (template1_icon_bitmap) {
    gbitmap_destroy(template1_icon_bitmap);
  }
  text_layer_destroy(template1_value_layer);
  bitmap_layer_destroy(template1_icon_layer);

  if (template2_icon_bitmap) {
    gbitmap_destroy(template2_icon_bitmap);
  }
  text_layer_destroy(template2_value_layer);
  bitmap_layer_destroy(template2_icon_layer);

  if (template3_icon_bitmap) {
    gbitmap_destroy(template3_icon_bitmap);
  }
  text_layer_destroy(template3_value_layer);
  bitmap_layer_destroy(template3_icon_layer);

  if (template4_icon_bitmap) {
    gbitmap_destroy(template4_icon_bitmap);
  }
  text_layer_destroy(template4_value_layer);
  bitmap_layer_destroy(template4_icon_layer);

  if (template5_icon_bitmap) {
    gbitmap_destroy(template5_icon_bitmap);
  }
  text_layer_destroy(template5_value_layer);
  bitmap_layer_destroy(template5_icon_layer);

  if (template6_icon_bitmap) {
    gbitmap_destroy(template6_icon_bitmap);
  }
  text_layer_destroy(template6_value_layer);
  bitmap_layer_destroy(template6_icon_layer);

  layer_destroy(hub_layer);
}

void bluetooth_handler(bool connected) {
	//	This handler is called when BT connection state changes
	if (connected) {
		#ifdef PBL_PLATFORM_APLITE
			if (inv_layer)
				inverter_layer_destroy(inv_layer);
		#endif	
		set_color();
	}
	else {
		#ifdef PBL_PLATFORM_APLITE
			inv_layer = inverter_layer_create(GRect(0, 0, 144, 168));
			layer_add_child(window_get_root_layer(window), (Layer*) inv_layer);	
		#else
			top_color = GColorWhite;
			time_color = GColorBlack;
		
			window_set_background_color(window, top_color);
    		text_layer_set_text_color(time_layer, time_color);
    		text_layer_set_text_color(date_layer, time_color);
		#endif
		vibes_double_pulse();
	}
}

static void init() {
  window = window_create();
  #ifdef PBL_SDK_2
	  window_set_fullscreen(window, true);
  #endif
  window_set_background_color(window, GColorBlack);
  font_date = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_CONDENSED_53));
  large_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_35));
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
  const bool animated = true;
  window_stack_push(window, animated);

  // Update the screen right away
  time_t now = time(NULL);
  handle_tick(localtime(&now), MINUTE_UNIT | DAY_UNIT );
  // And then every minute
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

  bluetooth_handler(bluetooth_connection_service_peek());
  bluetooth_connection_service_subscribe( bluetooth_handler );
  update_configuration();
	
  battery_state_service_subscribe(&battery_handler);
  battery_handler(battery_state_service_peek());
}

static void deinit() {
  battery_state_service_unsubscribe();
  window_destroy(window);
  tick_timer_service_unsubscribe();
  fonts_unload_custom_font(font_time);
  fonts_unload_custom_font(large_font);
	
  bluetooth_connection_service_unsubscribe();
}

int main(void) {
  setlocale(LC_ALL, "");
  qtp_conf = QTP_K_AUTOHIDE | QTP_K_INVERT;
  init();
  app_event_loop();
  deinit();
}

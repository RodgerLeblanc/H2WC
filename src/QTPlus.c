#include "QTPlus.h"

/* Initialize listeners to show and hide Quick Tap Plus as well as update data */
void qtp_setup() {
        qtp_is_showing = false;
        accel_tap_service_subscribe(&qtp_tap_handler);
}

/* Handle taps from the hardware */
void qtp_tap_handler(AccelAxisType axis, int32_t direction) {
        if (qtp_is_showing) {
                qtp_hide();
        } else {
                qtp_show();
        }
        qtp_is_showing = !qtp_is_showing;
}

/* Subscribe to taps and pass them to the handler */
void qtp_click_config_provider(Window *window) {
        window_single_click_subscribe(BUTTON_ID_BACK, qtp_back_click_responder);
}

/* Unusued. Subscribe to back button to exit */
void qtp_back_click_responder(ClickRecognizerRef recognizer, void *context) {
        qtp_hide();
}

/* Update the text layer for the Pebble battery status */
void qtp_update_battery_status(bool mark_dirty) {
        BatteryChargeState charge_state = battery_state_service_peek();
        static char battery_text[] = "Pebble: 100%";
        snprintf(battery_text, sizeof(battery_text), "Pebble: %d%%", charge_state.charge_percent);

        text_layer_set_text(qtp_battery_text_layer, battery_text);
        if (mark_dirty) {
                layer_mark_dirty(text_layer_get_layer(qtp_battery_text_layer));
        }
}

/* Update the text layer for the phone battery status */
void qtp_update_phone_battery_status() {
        static char battery_text[] = "Phone: 100%";
        snprintf(battery_text, sizeof(battery_text), "Phone: %d%%", qtp_phone_percent);

        text_layer_set_text(qtp_phone_battery_text_layer, battery_text);
}

/* Update the text layer for the calendar status */
void qtp_update_calendar_status() {
        text_layer_set_text(qtp_calendar_text_layer, qtp_calendar_events);
}

/* Auto-hide the window after a certain time */
void qtp_timeout() {
        qtp_hide();
        qtp_is_showing = false;
}

/* Create the QTPlus Window and initialize the layres */
void qtp_init() {
        qtp_window = window_create();
	
        /* Pebble Battery Status text layer */
        GRect battery_frame = GRect( 0, 0, QTP_SCREEN_WIDTH, 28 );
        qtp_battery_text_layer =  text_layer_create(battery_frame);
        text_layer_set_font(qtp_battery_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
		text_layer_set_text_alignment(qtp_battery_text_layer, GTextAlignmentCenter);
        qtp_update_battery_status(false);
        layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(qtp_battery_text_layer));
	
        /* Phone Battery Status text layer */
        GRect phone_battery_frame = GRect( 0, 30, QTP_SCREEN_WIDTH, 28 );
        qtp_phone_battery_text_layer =  text_layer_create(phone_battery_frame);
        text_layer_set_font(qtp_phone_battery_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
		text_layer_set_text_alignment(qtp_phone_battery_text_layer, GTextAlignmentCenter);
        qtp_update_phone_battery_status();
        layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(qtp_phone_battery_text_layer));
		
        /* Calendar text layer */
        GRect calendar_frame = GRect( 0, 63, QTP_SCREEN_WIDTH, 168-63 );
        qtp_calendar_text_layer =  text_layer_create(calendar_frame);
        text_layer_set_font(qtp_calendar_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
		text_layer_set_text_alignment(qtp_calendar_text_layer, GTextAlignmentCenter);
        qtp_update_calendar_status();
        layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(qtp_calendar_text_layer));

		/* Invert the screen */
        if (qtp_is_invert()) {
        }

        /* Register for back button */
        //window_set_click_config_provider(qtp_window, (ClickConfigProvider)qtp_click_config_provider);

}


/* Deallocate QTPlus items when window is hidden */
void qtp_deinit() {
        text_layer_destroy(qtp_battery_text_layer);
        text_layer_destroy(qtp_phone_battery_text_layer);
        text_layer_destroy(qtp_calendar_text_layer);
	
        if (qtp_is_invert()) {
        }
        window_destroy(qtp_window);
        if (qtp_is_autohide()) {
                app_timer_cancel(qtp_hide_timer);
				qtp_hide_timer = NULL;
        }
}

/* Deallocate persistent QTPlus items when watchface exits */
void qtp_app_deinit() {
	accel_tap_service_unsubscribe();
}

/* Create window, layers, text. Display QTPlus */
void qtp_show() {
        qtp_init();
        window_stack_push(qtp_window, true);
        if (qtp_is_autohide()) {
                qtp_hide_timer = app_timer_register(QTP_WINDOW_TIMEOUT, qtp_timeout, NULL);
        }
}

/* Hide QTPlus. Free memory */
void qtp_hide() {
        window_stack_pop(true);
        qtp_deinit();
}

bool qtp_is_autohide() {
        return true;
}

bool qtp_is_invert() {
        return true;
}

#include "main.h"
#include "firmware.h"

#include "display.h"
#include "settings.h"
#include "utils.h"

#include "scripts.h"

int *feedback_task = NULL;

type_CAMERA_MODE st_cameraMode;

void script_start();
void script_stop();
void script_feedback();

void script_shot(type_SHOT_ACTION action);
void sub_ext_aeb();
void sub_efl_aeb();
void sub_iso_aeb();

void script_delay(int seconds);

void script_ext_aeb() {
	script_start();

	if (settings.eaeb_delay)
		script_delay(2);

	if (status.script_running)
		script_shot(SHOT_ACTION_EXT_AEB);

	script_stop();

	status.last_script = SCRIPT_EXT_AEB;
}

void script_efl_aeb() {
	script_start();

	if (settings.eaeb_delay)
		script_delay(2);

	if (status.script_running)
		script_shot(SHOT_ACTION_EFL_AEB);

	script_stop();

	status.last_script = SCRIPT_EFL_AEB;
}

void script_iso_aeb() {
	script_start();

	if (settings.eaeb_delay)
		script_delay(2);

	if (status.script_running)
		script_shot(SHOT_ACTION_ISO_AEB);

	script_stop();

	status.last_script = SCRIPT_ISO_AEB;
}

void script_interval() {
	int i = 0;

	script_start();

	if (settings.interval_delay)
		script_delay(2);

	for (;;) {
		if (!status.script_running)
			break;

		script_shot(settings.interval_action);

		if (++i < settings.interval_shots || settings.interval_shots == 0)
			script_delay(settings.interval_time);
		else
			break;
	}

	script_stop();

	status.last_script = SCRIPT_INTERVAL;
}

void script_wave() {
	script_start();

	// First, wait for the sensor to be free, just in case
	while (status.script_running && FLAG_FACE_SENSOR)
		SleepTask(WAIT_USER_ACTION);

	do {
		// Now, wait until something blocks the sensor
		while (status.script_running && !FLAG_FACE_SENSOR)
			SleepTask(WAIT_USER_ACTION);

		// If instant not activated, wait until sensor is free again
		if (!settings.wave_instant) {
			while (status.script_running && FLAG_FACE_SENSOR)
				SleepTask(WAIT_USER_ACTION);
		}

		// Do the optional delay
		if (settings.wave_delay)
			script_delay(2);

		// And finally fire the camera
		if (status.script_running)
			script_shot(settings.wave_action);
	} while (status.script_running && settings.wave_repeat);

	script_stop();

	status.last_script = SCRIPT_WAVE;
}

void script_self_timer() {
	script_start();

	script_delay(settings.timer_timeout);

	if (status.script_running)
		script_shot(settings.timer_action);

	script_stop();

	status.last_script = SCRIPT_TIMER;
}

void script_start() {
	beep();
	status.script_running = TRUE;

	st_cameraMode = *cameraMode;

	send_to_intercom(IC_SET_CF_MIRROR_UP_LOCK, 1, FALSE);

	if (settings.dim_lcd_down)
		send_to_intercom(IC_SET_LCD_BRIGHTNESS, 1, 1);

	if (settings.keep_power_on)
		send_to_intercom(IC_SET_AUTO_POWER_OFF, 1, FALSE);

	if (feedback_task == NULL)
		feedback_task = CreateTask("Feedback", 5, 0x2000, script_feedback, 0);
	else
		UnSuspendTask(feedback_task);
}

void script_stop() {
	beep();
	status.script_running = FALSE;

	send_to_intercom(IC_SET_AUTO_POWER_OFF,    1, st_cameraMode.auto_power_off);
	send_to_intercom(IC_SET_CF_MIRROR_UP_LOCK, 1, st_cameraMode.cf_mirror_up_lock);
	send_to_intercom(IC_SET_LCD_BRIGHTNESS,    1, st_cameraMode.lcd_brightness);
}

void script_feedback() {
	for (;;) {
		while (status.script_running) {
			led_flash(FEEDBACK_LENGTH);
			SleepTask(FEEDBACK_INTERVAL);
		}

		SuspendTask(feedback_task);
	}
}

void script_shot(type_SHOT_ACTION action) {
	int aeb = cameraMode->ae_bkt;
	send_to_intercom(IC_SET_AE_BKT, 1, 0x00);

	switch (action) {
	case SHOT_ACTION_SHOT:
		shutter_release();
		break;
	case SHOT_ACTION_EXT_AEB:
		sub_ext_aeb();
		break;
	case SHOT_ACTION_EFL_AEB:
		sub_efl_aeb();
		break;
	case SHOT_ACTION_ISO_AEB:
		sub_iso_aeb();
		break;
	default:
		break;
	}

	send_to_intercom(IC_SET_AE_BKT, 1, aeb);
}

void sub_ext_aeb() {
	if (cameraMode->tv_val == TV_VAL_BULB) {
		int tv_val;

		for (tv_val = settings.eaeb_tv_max; tv_val <= settings.eaeb_tv_min; tv_val = tv_next(tv_val)) {
			if (tv_val < 0x10) {
				send_to_intercom(IC_SET_TV_VAL, 1, TV_VAL_BULB);
				shutter_release_bulb(1 << (1 - (tv_val >> 3)));
			} else {
				send_to_intercom(IC_SET_TV_VAL, 1, tv_val);
				shutter_release();
			}

			if (!status.script_running)
				break;
		};

		send_to_intercom(IC_SET_TV_VAL, 1, TV_VAL_BULB);
	} else if (cameraMode->ae < AE_MODE_AUTO) {
		int tv_inc, av_inc;
		int tv_dec, av_dec;

		int tv_sep = 0x00, av_sep = 0x00;
		int frames = settings.eaeb_frames;

		if (cameraMode->ae == AE_MODE_TV) {
			// Fixed Tv, Variable Av
			av_sep = settings.eaeb_ev;
		} else {
			// Variable Tv, Fixed Av
			tv_sep = settings.eaeb_ev;
		}

		// First photo taken using default values
		shutter_release();
		frames--;

		// Just hope the camera does not change these too quickly
		tv_inc = tv_dec = status.measured_tv;
		av_inc = av_dec = status.measured_av;

		// Enter manual mode...
		send_to_intercom(IC_SET_AE, 1, AE_MODE_M);

		// ...and do the rest ourselves
		while(frames) {
			if (settings.eaeb_direction == EAEB_DIRECTION_BOTH || settings.eaeb_direction == EAEB_DIRECTION_DOWN) {
				tv_inc = tv_add(tv_inc, tv_sep);
				av_inc = av_add(av_inc, av_sep);

				send_to_intercom(IC_SET_TV_VAL, 1, tv_inc);
				send_to_intercom(IC_SET_AV_VAL, 1, av_inc);

				shutter_release();
				frames--;

				if (!status.script_running)
					break;
			}

			if (settings.eaeb_direction == EAEB_DIRECTION_BOTH || settings.eaeb_direction == EAEB_DIRECTION_UP) {
				tv_dec = tv_sub(tv_dec, tv_sep);
				av_dec = tv_sub(av_dec, av_sep);

				send_to_intercom(IC_SET_TV_VAL, 1, tv_dec);
				send_to_intercom(IC_SET_AV_VAL, 1, av_dec);

				shutter_release();
				frames--;

				if (!status.script_running)
					break;
			}
		}

		// Restore values
		send_to_intercom(IC_SET_AE,     1, st_cameraMode.ae);
		send_to_intercom(IC_SET_TV_VAL, 1, st_cameraMode.tv_val);
		send_to_intercom(IC_SET_AV_VAL, 1, st_cameraMode.av_val);
	}
}


void sub_iso_aeb() {
	int i;

	for (i = 0; i < 5; i++) {
		if (settings.iso_aeb[i]) {
			send_to_intercom(IC_SET_ISO, 2, 0x40 | ((i + 1) << 3));
			SleepTask(WAIT_USER_ACTION);
			shutter_release();

			if (!status.script_running)
				break;
		}
	}

	send_to_intercom(IC_SET_ISO, 2, st_cameraMode.iso);
}

void sub_efl_aeb() {
	int frames = settings.efl_aeb_frames;

	int ef_inc = cameraMode->efcomp;
	int ef_dec = cameraMode->efcomp;

	shutter_release();
	frames--;

	while(frames) {
		if (settings.eaeb_direction == EAEB_DIRECTION_BOTH || settings.eaeb_direction == EAEB_DIRECTION_DOWN) {
			ef_inc = ev_add(ef_inc, settings.efl_aeb_ev);
			send_to_intercom(IC_SET_EFCOMP, 1, ef_inc);

			shutter_release();
			frames--;

			if (!status.script_running)
				break;
		}

		if (settings.eaeb_direction == EAEB_DIRECTION_BOTH || settings.eaeb_direction == EAEB_DIRECTION_UP) {
			ef_dec = ev_sub(ef_dec, settings.efl_aeb_ev);
			send_to_intercom(IC_SET_EFCOMP, 1, ef_dec);

			shutter_release();
			frames--;

			if (!status.script_running)
				break;
		}
	}

	send_to_intercom(IC_SET_EFCOMP, 1, st_cameraMode.efcomp);
}

void script_delay(int seconds) {
	int i;

	while (seconds--) {
		for (i = 0; i < SCRIPT_DELAY_REPEAT; i++) {
			SleepTask(SCRIPT_DELAY_TIME);
			if (!status.script_running)
				return;
		}

	}

	SleepTask(SCRIPT_DELAY_TIME);
}

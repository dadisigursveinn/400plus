#include "main.h"
#include "firmware.h"

#include "utils.h"

#include "settings.h"

type_SETTINGS settings = {
	iso_in_viewfinder: FALSE,
	autoiso_enable   : FALSE,
	autoiso_miniso   : 0x48, // ISO100
	autoiso_maxiso   : 0x68, // ISO1600
	autoiso_mintv    : 0x68, // 1/60s
	autoiso_maxav    : 0x28, // f/4.0
	eaeb_delay       : FALSE,
	eaeb_frames      : 3,
	eaeb_ev          : 0x08, // 1EV
	eaeb_tv_min      : 0x78, // 1/250s
	eaeb_tv_max      : 0x68, // 1/60s
	eaeb_direction   : EAEB_DIRECTION_BOTH,
	efl_aeb_delay    : FALSE,
	efl_aeb_frames   : 3,
	efl_aeb_ev       : 0x08, // 1EV
	efl_aeb_direction: EAEB_DIRECTION_BOTH,
	iso_aeb_delay    : FALSE,
	iso_aeb          : {TRUE, TRUE, TRUE, TRUE, TRUE},
	interval_delay   : FALSE,
	interval_time    : 2,
	interval_action  : SHOT_ACTION_FIRST,
	interval_shots   : 0,
	wave_delay       : FALSE,
	wave_action      : SHOT_ACTION_FIRST,
	wave_repeat      : FALSE,
	wave_instant     : FALSE,
	lexp_delay       : FALSE,
	lexp_time        : 60,
	remote_delay     : FALSE,
	timer_timeout    : 5,
	timer_action     : SHOT_ACTION_FIRST,
	main_order       : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	params_order     : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	shortcuts_order  : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	scripts_order    : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	info_order       : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	developer_order  : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	settings_order   : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ,13, 14},
	dim_lcd_down     : TRUE,
	keep_power_on    : TRUE,
	debug_on_poweron : FALSE,
	logfile_mode     : 0,
	remote_enable    : FALSE,
	developers_menu  : FALSE,
	button_jump      : BUTTON_ACTION_ISO,
	button_disp      : FALSE,
	language         : 0,
};

int settings_read() {
	int result  = FALSE;

	int file    = -1;
	int version =  0;

	type_SETTINGS buffer;

	if ((file = FIO_OpenFile(SETTINGS_FILE, O_RDONLY, 644)) == -1)
		goto end;

	if (FIO_ReadFile(file, &version, sizeof(version)) != sizeof(version))
		goto end;

	if (version != SETTINGS_VERSION)
		goto end;

	if (FIO_ReadFile(file, &buffer, sizeof(buffer)) != sizeof(buffer))
		goto end;

	settings = buffer;
	result   = TRUE;

end:
	if (file != -1)
		FIO_CloseFile(file);

	return result;
}

void settings_write() {
	const int version = SETTINGS_VERSION;
	int file = FIO_OpenFile(SETTINGS_FILE, O_CREAT | O_WRONLY , 644);

	if (file != -1) {
		FIO_WriteFile(file, (void*)&version, sizeof(version));
		FIO_WriteFile(file, &settings, sizeof(settings));
		FIO_CloseFile(file);
	}
}

extern void settings_apply() {
	if (settings.remote_delay) {
		RemReleaseSelfMax = 4500;
		RemReleaseInstMin = 5560;
	} else {
		RemReleaseSelfMax = 6160;
		RemReleaseInstMin = 7410;
	}

	if (settings.remote_enable) {
		remote_on();
	} else {
		remote_off();
	}
}
/**
 * $Revision$
 * $Date$
 * $Author$
 */

#include <unistd.h>
#include <stdbool.h>

#include "macros.h"

#include "cmodes.h"
#include "display.h"
#include "languages.h"
#include "menu.h"
#include "menupage.h"
#include "menuitem.h"
#include "settings.h"
#include "utils.h"

#include "menu_settings.h"

extern char languages_found[MAX_LANGUAGES][LP_MAX_WORD];

void menu_settings_open();

void menu_restore_settings();
void menu_restore_cmodes  ();
void menu_delete_cmodes   ();

void reload_language_and_refresh(const type_MENUITEM *item);

type_MENUITEM scripts_items[] = {
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_KEEP_POWER_ON), &settings.keep_power_on,    NULL),
	MENUITEM_SCRLCD( 0, LP_WORD(L_I_LCD_SCRIPT),    &settings.script_lcd,       NULL),
	MENUITEM_SCRIND( 0, LP_WORD(L_I_INDICATOR),     &settings.script_indicator, NULL),
};

type_MENUITEM buttons_items[] = {
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_USE_DPAD),    &settings.use_dpad,     NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_BUTTON_DISP), &settings.button_disp,  NULL),
	MENUITEM_BTNACTN(0, LP_WORD(L_I_BTN_JUMP),    &settings.button_jump,  NULL),
	MENUITEM_BTNACTN(0, LP_WORD(L_I_BTN_TRASH),   &settings.button_trash, NULL),
};

type_MENUITEM cmodes_items[] = {
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_PRESETS_CAMERA),   &cmodes_config.recall_camera,   NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_PRESETS_400PLUS),  &cmodes_config.recall_400plus,  NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_PRESETS_ORDERING), &cmodes_config.recall_ordering, NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_PRESETS_SETTINGS), &cmodes_config.recall_settings, NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_PRESETS_IMAGE),    &cmodes_config.recall_image,    NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_PRESETS_CFN),      &cmodes_config.recall_cfn,      NULL),
};

type_MENUITEM menus_items[] = {
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_WRAP_MENUS),    &settings.menu_wrap,      NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_NAVIGATE_MAIN), &settings.menu_navmain,   NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_ENTER_MAIN),    &settings.menu_entermain, NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_AUTOSAVE),      &settings.menu_autosave,  NULL),
};

type_MENUITEM pages_items[] = {
	MENUITEM_INFO(0, LP_WORD(L_P_PARAMS),     NULL),
	MENUITEM_INFO(0, LP_WORD(L_P_SCRIPTS),    NULL),
	MENUITEM_INFO(0, LP_WORD(L_P_INFO),       NULL),
	MENUITEM_INFO(0, LP_WORD(L_P_SETTINGS),   NULL),
	MENUITEM_INFO(0, LP_WORD(L_P_PRESETS),    NULL),
};

type_MENUITEM restore_items[] = {
	MENUITEM_LAUNCH(0, LP_WORD(L_I_RESTORE_SETTINGS), menu_restore_settings),
	MENUITEM_LAUNCH(0, LP_WORD(L_I_RESTORE_PRESETS),  menu_restore_cmodes  ),
	MENUITEM_LAUNCH(0, LP_WORD(L_I_DELETE_PRESETS),   menu_delete_cmodes   ),
};

type_MENUPAGE scripts_page = {
	name    : LP_WORD(L_S_SCRIPTS),
	length  : LENGTH(scripts_items),
	items   : scripts_items,
	actions : {
		[MENU_EVENT_AV]   = menu_return,
	}
};

type_MENUPAGE buttons_page = {
	name    : LP_WORD(L_S_BUTTONS),
	length  : LENGTH(buttons_items),
	items   : buttons_items,
	actions : {
		[MENU_EVENT_AV]   = menu_return,
	}
};

type_MENUPAGE cmodes_page = {
	name    : LP_WORD(L_S_PRESETS),
	length  : LENGTH(cmodes_items),
	items   : cmodes_items,
	actions : {
		[MENU_EVENT_AV]   = menu_return,
	}
};

type_MENUPAGE menus_page = {
	name    : LP_WORD(L_S_MENUS),
	length  : LENGTH(menus_items),
	items   : menus_items,
	actions : {
		[MENU_EVENT_AV]   = menu_return,
	}
};

type_MENUPAGE pages_page = {
	name     : LP_WORD(L_S_PAGES),
	length   : LENGTH(pages_items),
	items    : pages_items,
	ordering : menu_order.main_order,
	actions  : {
		[MENU_EVENT_AV]   = menu_return,
	}
};

type_MENUPAGE restore_page = {
	name     : LP_WORD(L_I_RESTORE),
	length   : LENGTH(restore_items),
	items    : restore_items,
	actions  : {
		[MENU_EVENT_AV]   = menu_return,
	}
};

type_MENUITEM menu_settings_items[] = {
	MENUITEM_LANG   (0, LP_WORD(L_I_LANGUAGE),         &settings.language,        reload_language_and_refresh),
	MENUITEM_SUBMENU(0, LP_WORD(L_S_SCRIPTS),          &scripts_page,             NULL),
	MENUITEM_SUBMENU(0, LP_WORD(L_S_BUTTONS),          &buttons_page,             NULL),
	MENUITEM_SUBMENU(0, LP_WORD(L_S_PRESETS),          &cmodes_page,             NULL),
	MENUITEM_SUBMENU(0, LP_WORD(L_S_MENUS),            &menus_page,               NULL),
	MENUITEM_SUBMENU(0, LP_WORD(L_S_PAGES),            &pages_page,               NULL),
	MENUITEM_SUBMENU(0, LP_WORD(L_I_RESTORE),          &restore_page,             NULL),
	MENUITEM_BOOLEAN(0, LP_WORD(L_I_DEVELOPERS_MENU),  &settings.developers_menu, NULL),
};

type_MENUPAGE menupage_settings = {
	name      : LP_WORD(L_P_SETTINGS),
	sibilings : true,
	length    : LENGTH(menu_settings_items),
	items     : menu_settings_items,
	ordering  : menu_order.settings_order,
	actions   : {
		[MENU_EVENT_OPEN] = menu_settings_open,
	}
};

void reload_language_and_refresh(const type_MENUITEM *item) {
	lang_pack_config();
	menu_event_display();
}

void menu_settings_open() {
	int i;

	for (i = 0; i<MAX_LANGUAGES && languages_found[i] != '\0' && languages_found[i][0] != '\0'; i++) {
		menupage_settings.items[0].parm.menuitem_enum.list->length  = i + 1;
		menupage_settings.items[0].parm.menuitem_enum.list->data[i] = languages_found[i];
	}
}

void menu_restore_settings() {
	settings_restore();
	menu_return();
	beep();
}

void menu_restore_cmodes  () {
	cmodes_restore();
	menu_return();
	beep();
}

void menu_delete_cmodes   () {
	cmodes_delete();
	menu_return();
	beep();
}

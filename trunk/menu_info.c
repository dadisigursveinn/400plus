/**
 * $Revision$
 * $Date$
 * $Author$
 */

#include <stdbool.h>

#include "macros.h"
#include "firmware.h"

#include "languages.h"
#include "menu.h"
#include "menupage.h"
#include "menuitem.h"
#include "settings.h"

#include "menu_info.h"

menuitem_t menupage_info_items[] = {
	MENUITEM_INFO (0, LP_WORD(L_I_VERSION),        VERSION),
	MENUITEM_PARAM(0, LP_WORD(L_I_RELEASE_COUNT), &FLAG_RELEASE_COUNT),
	MENUITEM_PARAM(0, LP_WORD(L_I_BODY_ID),       &FLAG_BODY_ID),
	MENUITEM_INFO (0, LP_WORD(L_I_FIRMWARE),       FIRMWARE_VERSION),
//	MENUITEM_INFO (0, LP_WORD(L_I_MODEL),          MODEL_NAME),
	MENUITEM_INFO (0, LP_WORD(L_I_OWNER),          OWNER_NAME),
};

menupage_t menupage_info = {
	name        : LP_WORD(L_P_INFO),
	sibilings   : true,
	length      : LENGTH(menupage_info_items),
	items       : menupage_info_items,
	ordering    : menu_order.info,
};

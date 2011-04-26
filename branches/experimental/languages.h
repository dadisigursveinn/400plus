#ifndef LANGUAGES_H_
#define LANGUAGES_H_

#define MAX_LANG_WORD 32
#define LP_WORD(word) LangPlus_current[word]
enum LangPlus_id {
	L_FREE_SPACE,		// 12 chars max
	L_RELEASE_COUNT,	// 12 chars max
	L_YES,
	L_NO,
	L_2S,			// 2s (two seconds)
	L_OFF,
	L_ENABLED,
	L_DISABLED,
	L_EXT_ONLY,
	L_ONE_SHOT,
	L_EXT_AEB,
	L_INTERVAL,
	L_NO_LIMIT,

	L_DEVELOPER,
	L_ENTER_FACTORY_MODE,	// 18 chars max
	L_EXIT_FACTORY_MODE,	// 18 chars max
	L_START_DEBUG_MODE,	// 18 chars max

	L_LOAD_PRESETS,
	L_LOAD_PRESET_1,
	L_LOAD_PRESET_2,
	L_LOAD_PRESET_3,
	L_LOAD_PRESET_4,
	L_LOAD_PRESET_5,
	L_SAVE_PRESETS,
	L_SAVE_PRESET_1,
	L_SAVE_PRESET_2,
	L_SAVE_PRESET_3,
	L_SAVE_PRESET_4,
	L_SAVE_PRESET_5,

	L_SETTINGS,
	L_DELAY,
	L_ACTION,
	L_REPEAT,
	L_INSTANT,
	L_FRAMES,
	L_STEP_EV,
	L_MANUAL_L,
	L_MANUAL_R,
	L_TIME_S,
	L_EAEB,
	L_SHOTS,
	L_AV_COMP,
	L_FLASH_COMP,
	L_AEB,
	L_ISO_IN_VF,
	L_SHORTCUTS_MENU,
	L_SAFETY_SHIFT,
	L_COLOR_TEMP_K,
	L_USE_FLASH,
	L_HANDWAVE,
	L_TIMER_SPACES,
	L_IR_REMOTE_DELAY,
	L_DEVELOPERS_MENU,

	L_SHORTCUTS,
	L_ISO,
	L_EXTENDED_AEB,
	L_INTERVALOMETER,
	L_HAND_WAVING,
	L_SELF_TIMER,
	L_AF_FLASH,
	L_MIRROR_LOCKUP,
	L_FLASH_2ND_CURT,

	L_EOL
};
extern char * LangPlus_lang_packs[16][L_EOL];
extern char LangPlus_current[L_EOL][MAX_LANG_WORD];
extern int LangPlus_last_langid;

void LangPlus_lang_packs_init();


#endif // LANGUAGES_H_
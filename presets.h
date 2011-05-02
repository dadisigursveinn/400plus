#ifndef PRESETS_H_
#define PRESETS_H_

#define PRESETS_VERSION 0x01
#define PRESETS_CONFIG  "A:/PRESETS"
#define PRESETS_FILE    "A:/PRESET_%u"

typedef struct {
	type_SETTINGS    settings;
	type_CAMERA_MODE camera_mode;
} type_PRESET;

typedef struct {
	int  order[5];
	char names[5][32];
} type_PRESETS_CONFIG;

extern type_PRESETS_CONFIG presets_config;

extern void presets_read();
extern void presets_write();

extern int  preset_read (int id);
extern void preset_write(int id);
extern void preset_apply();
extern void preset_recall();

#endif /* PRESETS_H_ */

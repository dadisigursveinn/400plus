#ifndef DEBUG_H_
#define DEBUG_H_

#include "firmware.h"
#include "mainctrl.h"

typedef enum {
	DEBUG_GENERIC    = 0x00, // +SFACT
	DEBUG_MAIN       = 0x01, // +MC +TEST
	DEBUG_STM_SSM    = 0x08, // +AvailShots +Lens +AVS
	DEBUG_SCP        = 0x09, // +SSMain
	DEBUG_SDV        = 0x0A, // develop?
	DEBUG_SRC        = 0x0B,
	DEBUG_GUI        = 0x18,

} canon_debug_class_t;


#ifdef ENABLE_DEBUG

#define debug_log(f, p...) printf_log(8, 8, "[400Plus] %s[%d]: " f, __FILE__, __LINE__, ##p)
#define debug_printf(f, p...) printf("\n[400Plus] %s[%d]: " f, __FILE__, __LINE__, ##p)

#define LEDRED (*((int*)0xC02200A0))
#define LEDBLUE (*((int*)0xC0220000))
#define LEDON   0x46
#define LEDOFF  0x44

#define blink_cycles 1000000
#define blink_red() do { int i; LEDRED=LEDON; for (i=0;i<blink_cycles; i++); LEDRED=LEDOFF; for (i=0;i<blink_cycles; i++); } while (0)
#define blink_blue() do { int i; LEDBLUE=LEDON; for (i=0;i<blink_cycles; i++); LEDBLUE=LEDOFF; for (i=0;i<blink_cycles; i++); } while(0)

#else // ENABLE_DEBUG

#define debug_log(...)
#define debug_printf(...)

#endif


#endif // DEBUG_H_

/**
 * $Revision$
 * $Date$
 * $Author$
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "scripts.h"

#define TRUE  1
#define FALSE 0

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define INT_SWAP(x, y) do {int _SWAP_; _SWAP_=(x); (x)=(y); (y)=_SWAP_;} while(0)

#define LENGTH(array) (sizeof(array) / sizeof(array[0]))

// Action definitions
typedef void(*type_TASK)();

typedef struct {
	int       button;
	int       block;
	type_TASK task[2];
	int       _eol_;
} type_ACTION;

typedef struct {
	int          type;      // GUIMode for intercom_proxy(), type_MENU_TYPE for menu_buttons_handler()
	type_ACTION *actions;
	int          _eol_;
} type_CHAIN;

// Global status
typedef struct {
	int         button_down;       // A button is down, and which one
	int         script_running;    // A script is running
	int         script_stopping;   // The currently running script mist stop
	int         menu_running;      // A menu is running
	int         iso_in_viewfinder; // ISO is being displayed in the viewfinder
	int         afp_dialog;        // The last active dialog was the AF Point selection dialog
	int         main_dial_ae;      // AE mode selected in the main dial
	type_TASK   button_up_task;    // Task that must be executed when the current button is released
	int         button_up_block;   // Response when the current button is released
	int         last_preset;       // Last preset used
	int         ignore_ae_change;  // Ignore next AE change
	int         booting;           // Camera is still booting up
	int         measuring;         // Camera is measuring the scene
	int         measured_tv;       // Shutter speed as proposed by the metering
	int         measured_av;       // Aperture as proposed by the metering
	int         measured_ev;       // Exposure deviation as measured by the camera
	int         last_shot_tv;      // Shutter speed of the last shot taken
	int         last_shot_av;      // Aperture of the last shot taken
	int         ev_comp;           // Exposure compensation for AutoISO + M
	type_SCRIPT last_script;       // Last executed script
} type_STATUS;

// Main message queue
extern int *task_queue;

// Inline code
#define ENQUEUE_TASK(task) TryPostMessageQueue(task_queue, (task), FALSE);

#define END_OF_LIST  {_eol_ : TRUE}
#define IS_EOL(item) (item->_eol_)

// Our own code
extern void initialize();
extern void initialize_display();
extern void intercom_proxy(const int handler, char *message);

// Shared globals
extern type_STATUS status;

#endif /* MAIN_H_ */

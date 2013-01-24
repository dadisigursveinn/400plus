/**
 * $Revision$
 * $Date$
 * $Author$
 *
 * Fixed exposure for M mode
 *
 */

#include <stdbool.h>

#include "main.h"
#include "macros.h"
#include "camera.h"
#include "firmware.h"

#include "exposure.h"
#include "utils.h"

#include "fexp.h"

void fexp_disable () {
	if (status.fexp) {
		status.fexp = false;

		beep();
	}
}

void fexp_toggle() {
	if (status.fexp || DPData.ae != AE_MODE_M) {
		fexp_disable();
	} else {
		status.fexp    = true;
		status.fexp_ev = (int)DPData.av_val + (int)DPData.tv_val;

		beep();
	}
}

void fexp_update_av() {
	int av = status.fexp_ev - DPData.tv_val;

	av = CLAMP(av, AV_MIN, AV_MAX);
	av = ev_normalize(av);

	send_to_intercom(IC_SET_AV_VAL, av);
}

void fexp_update_tv() {
	int tv = status.fexp_ev - DPData.av_val;

	tv = CLAMP(tv, TV_MIN, TV_MAX);
	tv = ev_normalize(tv);

	send_to_intercom(IC_SET_TV_VAL, tv);
}

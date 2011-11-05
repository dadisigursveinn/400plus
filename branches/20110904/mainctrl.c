#include "main.h"
#include "mainctrl.h"
#include "firmware.h"
#include "tasks.h"
#include "debug.h"

typedef struct MC_Table_entry_struct {
	mc_event_t t;
	int sw;
	int arg;
} mc_table_t;

void my_MC_T_Button(mc_table_t * event) {

	// actually, i feel a little bit stupid now...
	// i was so enthusiastic about task_mainctrl() and mc_button() routines
	// that i missed the important thing...
	// we could just let the original one do the stuff...
	//	MC_T_Button(event);
	//
	// and use this routine as a proxy, which would block or pass the event...
	// just like the intecom_proxy
	//
	// of course it is better to know what is going on arround, and this helped me to find
	// some buttons which we didnt knew before, but still i could save myself one night...


	printf_log(1, 6, asw04d04d /*"\t sw:%04d(%04d)"*/, event->sw, event->arg);
	//static char buf[30];
	//printf_log(1, 6, "\t BUTTON: sw:0x%02X(%s) arg:(0x%04X)", event->sw, mc_btn_name(buf, event->sw), event->arg);

	switch (event->sw) {

	case MC_BUTTON_BATTERY_DOOR_OPEN: // 162+13 = 175 // bat door
		if (event->arg == 0) {
			err_MC_T |= 2;
			CardDoorEmergency_Func();
		}
		break;
	case MC_BUTTON_CARD_DOOR_OPEN: // 162+9 = 171 // card door
		if (event->arg == 0) {
			err_MC_T |= 1;
			CardDoorEmergency_Func();
		}
		break;

	case MC_BUTTON_POWER: // 162+12 = 174 // shutdown
		if (event->arg !=0) {
			err_MC_T = err_MC_T & ~4;
			break;
		} else {
			err_MC_T |= 4;
			if (*(some_important_structure + 0xBC) != 1) { // off_24860.0xBC -> offset in some structure
				change_menupos();
			}

			if (unk_258A0 != 0) { // i think this flag indicates that we are taking photo ATM
				set_dword_7610_to_0();
				DDD_Capture(event->arg);// now we need to find WTF is this DDD... i see this DDD stuff frequently in the FW.
				// when DDD_Capture(), is called with arg==0 (like in this call)
				// it will call internaly End_DDD_Capture.
				// so i guess this is called when we turn off the camera, while it's shooting
				// to finish the taking of the photo
			}

			set_2A0E0_to_1();
			change_playback_file_id2();

			if (!(MC_State & 0x800)) { // i'm not 100% sure that i have to reverse the condition here, but it's not fatal
				SetTurnDisplayEvent_2_after_1();
			}
		}
		break;

	case MC_BUTTON_HALF_SHUTTER: // 162+20 = 182
		SendToIntercom(IC_BUTTON_HALF_SHUTTER, 0, 0);
		eventproc_RemOn();
		break;

	case MC_BUTTON_FULL_SHUTTER: // 162+21 = 183
		SendToIntercom(IC_BUTTON_FULL_SHUTTER, 0, 0);
		eventproc_RemOn();
		break;

	case MC_BUTTON_UNK2: // 162+26 = 188
		beep();SleepTask(500);beep(); // if you hear 2 beeps, please tell me which btn you've pressed.
		eventproc_TFTOff();
		break;

	case MC_BUTTON_JUMP: // 162+0 = 162 // btn JUMP
		switch (FLAG_GUI_MODE) {
		case GUIMODE_MAIN:
		case GUIMODE_OLC:
			ENQUEUE_TASK(set_intermediate_iso);
			break;
		default:
			MC_T_Button(event);
			break;
		}
		break;

	case MC_BUTTON_TRASH: // 162+3 = 165 // btn TRASH
		switch (FLAG_GUI_MODE) {
		case GUIMODE_MAIN:
		case GUIMODE_OLC:
			ENQUEUE_TASK(emulate_menu_button);
			break;
		default:
			MC_T_Button(event);
			break;
		}
		break;

	case MC_BUTTON_UNK1: // 162+10= 172 // btn UNK1
		//printf_log(1, 6, "AF: btn: %d, PF: %d,%d\n", event->sw, PowerFlag, (PowerFlag|1));
		//SendToIntercom(IC_POWER_FLAG, 1, (PowerFlag|1)); // im not sure why this isnt working ?
		MC_T_Button(event); // so we call the original btn handler to take care of it.
		break;

	default:
		ManySendToGUI_and_other(event->sw, event->arg); // perhaps it would be good to have this one
		// reversed too, but it would take much time, as it's a big one
		break;
	}
}

void my_task_MainCtrl() {

	// using printf() in this function makes troubles with shooting...
	// took me the whole eternity to understand that ...
	// we can use the canon's printf_log of course.
	// perhaps the direct writing to STDOUT.TXT conflicts with writing the photo to the CF...
	// after all we are redirecting the console to STDOUT.TXT, and the camera would not
	// expect that someone other than canon's routines will try to write to the CF.

	(*((int*)0xC0220000)) = 0x46; // turn on the blue led
	mc_table_t * event;

	while (1) {
		int msg;

		ReceiveMessageQueue(hMainMessQueue, &msg, 0);
		event = (mc_table_t *)(&MC_T_Table[msg*3]);

		printf_log(1, 6, aMcT04dS04xD, event->t, MC_State, msg); // "[MC] T:%04d, S:%04X, %d"

		if (0) {
		} else if (event->t < 6) {
			if (event->t == MC_BUTTON) {
				my_MC_T_Button(event); // we add our handler here
			} else {
				MC_T_1_5(event);
			}
		} else if (event->t < 8) {
			MC_T_6_7(event);
		} else if (event->t < 21) {
			MC_T_8_20(event);
		} else if (event->t < 26) {
			MC_T_21_25(event);
		} else if (event->t == 26) {
			InitializeDriveManager();
			InitializeDcfManager();
			RegisterChangeNotifyCallback_DCF();
			RegisterDriveNotifyCallBack(DriveNotifyCallBack, 0);
			RegisterChangeNotifyCallback(ChangeNotifyCallback_MC, 0);
			GiveSemaphore(hMainCtrlMonoSem);
		} else if (event->t == 27) {
			if (!(MC_State & 0b11000000)) {
				err_MC_T = event->t;
				ErrorCollectInstance(aMainCannotPowo); // "Main: Cannot PowOff at Emergency"
			}
		} else if (event->t == 28) {
			some_with_FixFacePos_0(event->sw);
		} else if (event->t == 29) {
			MC_T_29(event->sw);
		} else if (event->t == 30) {
			if (event->sw == 0) {
				some_with_FixFacePos_0(1);
				some_with_FixFacePos(1);
				SendToIntercom(IC_DIALOGOFF, 0, 0);
			} else {
				some_with_FixFacePos(0);
				FaceSensorOff();
				StartFaceSensor();
				sub_FF825078();
			}
		} else {
			printf("!!!!! should not be here, the OFW never checks this situation.\n");
		}

	} // while(1)
}


void my_MainCtrlInit() {
	hMainCtrlMonoSem = CreateBinarySemaphore(aMonoSem, 0);

	MC_dword_2A520 = 0;
	MC_dword_259C0 = 0;
	MC_State = 1;
	MC_dword_26940 = 0;
	MC_dword_27BE0 = 0;

	PictureStyleInit();
	sub_FF825AE4();
	SetEmergencyProc(proc_CardDoor_Emergency, 0);
	SetCardDoorProc(proc_CardDoor_Emergency, 0);
	TryPostMessageQueueFds_7(SendToMC_T_28, 0);
	SetErrorDetectActSweepProc(ErrorDetectActSweep);
	hMainMessQueue = (int*)CreateMessageQueue(aMainMessQueue, 0x64);
	hMainDataQueue = (int*)CreateMessageQueue(aMainDataQueue, 0xC8);
	CreateTask(aMainCtrl, 0x15, 0x4000, my_task_MainCtrl, 0);
	DebugProcsInit();
	CreateInterComQueue();
	MC_InitStart();
	IntercomHandlersInit();
	HandlersInit();
	ShootMainHandlerInit();
	sub_FF822DC0();
	sub_FF82296C();
	SetDprPrinterProperty();
	MC_RegisterEventProcedures();

	if (GetMainPreserveData_field_1C_LSR30()==0)
		goto loc_FF81BD8C;

	some_with_FixFacePos_0(0);

	unsigned char localvar1[0x198];
	unsigned char localvar2[0x194];

	memset(localvar1, 0, 0x198);
	localvar1[0x174] = 1;
	localvar2[0x174] = 1;

	SendMessageDprRequestChangeData(localvar1, localvar2);

loc_FF81BD8C:
	if (sub_FF823F34() != 0)
		goto out;

	TakeSemaphore(hMainCtrlMonoSem, 0);
	dword_1C78 = 1;

	if (get_0x1CCC() == 0)
		goto givesem;

	AbortDcfOperation();
	change_playback_file_id(1, 1);
	sub_FF823A44();

givesem:
	GiveSemaphore(hMainCtrlMonoSem);

out:
	return;
}
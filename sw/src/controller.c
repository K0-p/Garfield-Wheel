//Gamepad Device Definition
//Page 132 reference: https://www.ti.com/lit/ug/spmu297e/spmu297e.pdf?ts=1711565545552&ref_url=https%253A%252F%252Fwww.google.com%252F
//VID reference: https://www.fourwalledcubicle.com/files/LUFA/Doc/120730/html/_page__v_i_d_p_i_d.html

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stringtable.h"

#include "../inc/tm4c123gh6pm.h"
#include "../lib/usblib/device/usbdbulk.h"
#include "../lib/usblib/usblib.h"
#include "../lib/usblib/usbhid.h"
#include "../lib/usblib/device/usbdevice.h"
#include "../lib/usblib/device/usbdhid.h"
#include "../lib/usblib/device/usbdhidgamepad.h"


int main(void){
	USBDBulkInit(0, &g_sBulkDevice);
	return 1;
}
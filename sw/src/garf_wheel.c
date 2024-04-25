///**
// * @file garf_wheel.c
// * @authors: Gabriel Moore and Calvin Heischman
// * @brief
// *    This code turns the TM4C into a generic USB HID gamepad device that
// *    interfaces with the host like a steering wheel. It's gyro enabled
// *    and can play Garfield Kart. Cool shit overall <3
// * 
// *    Have fun garf karting!
// * 
// * @version 69.420
// * @date 04-12-2024
// *
// * @copyright Copyright (c) 2024
// * @note Potential Pinouts: (FILL OUT LATER)
// *    USB:
// *    Buttons (recommended garfield kart keybind):
// *				MODE 1 [1 pulse LED]: Full controls w/ gyro
// *				MODE 2 [2 pulse LED]:	No gryo, just use left/right buttons to steer with accelerate auto on
// *			Start: PD2	(bind to forwards)
// *			Reset: RST
// *			Up:		 PE3	(TOGGLE MODE  --  only impacts the following 3 buttons)
// *			Right: PB5	1:(bind to pause) 		2:(bind to right steering)
// *			Down:	 PE2	1:(bind to backwards) 2:(bind to toggle off/forwards)
// *			Left:	 PB4	1:(bind to drift) 		2:(bind to left steering)
// *    Gyro: (directions subject to change based on mounting)
// *			X:	Left-right steering
// *			Y:	Forward-back throw item
// *			Z:	*unused*	--	would overcomplicate controls
// *		ST7735:

// *    Garfield Binds:
// *			Left:		turn left
// *			Right:	turn right
// *			Start:	drive (TOGGLE)
// *			Up:			ability forward
// *			Right:	play/pause
// *			Down:		ability backward
// *			Left:		brake
// *			Back:		drift

// Copyright 2024 by Gabriel Moore & Calvin Heischman (copy our code if you dare, it's not even that good you dork)
#include "garf_handler.h"
#include "menu.h"
#include "switches.h"

enum garfstate g_iGamepadState;

// An activity counter to slow the LED blink down to a visible rate.
static uint32_t g_ui32Updates;
// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif
// Macro used to convert the 12-bit unsigned values to an eight bit signed value returned in the HID report.
// This maps the values from the ADC that range from 0 to 2047 over to 127 to -128.
#define Convert8Bit(ui32Value)  ((int8_t)((0x7ff - ui32Value) >> 4))

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"

#define PB4		(*((volatile uint32_t *)0x40005040))  
	
//	***	CHANGE BASED ON WHAT YOU'RE RUNNING ON	***	//
#define PCB false

struct buttholes_t prev_press;


int main(void){
		displayinit();	
		gyro_state = 1;
		uint32_t hold = 1;
    uint8_t ui8ButtonsChanged, ui8Buttons;
    bool bUpdate;
		//printf("PASS 1\n");
		prev_press.start = 1;prev_press.up = 1;prev_press.right = 1;prev_press.down = 1;prev_press.left = 1;
		//printf("PASS 2\n");
    usb_inits(PCB);	//inits PLL, GPIO, ADC/GYRO, UART, Buttons, and HID dependencies
		//printf("PASS 3\n");	
	

/* 	Main usb device handling:
			1) WAIT FOR HOST CONNECTION
			2) IF DISCONNECT: RETURN TO TOP AND WAIT FOR NEW CONNECTION	*/
    while(1){
				uint32_t ui32PortD = ROM_GPIOPinRead(GPIO_PORTD_AHB_BASE, GPIO_PIN_2);
				//uint32_t ui32PortD = 1;
        // Wait here until USB device is connected to a host.
				//printf("PASS MAIN\n");	
				if(PCB) g_iGamepadState = eStateIdle;
        if(g_iGamepadState == eStateIdle)
        {					
            // No update by default.
            bUpdate = false;
					
						headbounce();
						sReport.ui8Buttons = 0;
						//implementing non-board buttons
						//start: 0x4, urdl: 0x8,0x10,0x20,0x40
						struct buttholes_t assfingered = getbuttons(ui32PortD);
						//if buttons are different from previous state
						if((assfingered.start != prev_press.start)||(assfingered.up != prev_press.up)||(assfingered.right != prev_press.right)||(assfingered.down != prev_press.down)||(assfingered.left != prev_press.left)||(assfingered.left_L != prev_press.left_L)||(assfingered.right_R != prev_press.right_R)){
							//if ass is 1, release task
							if(assfingered.start == 0) sReport.ui8Buttons |= 0x4;
							if(assfingered.up == 0) sReport.ui8Buttons |= 0x8;
							if(assfingered.right == 0) sReport.ui8Buttons |= 0x10;
							if(assfingered.down == 0) sReport.ui8Buttons |= 0x20;
							if(assfingered.left == 0) sReport.ui8Buttons |= 0x40;
						if(assfingered.right_R == 0) sReport.ui8Buttons |= 0x1;
						if(assfingered.left_L == 0) sReport.ui8Buttons |= 0x2;
							uint32_t new_buttons = ((assfingered.start << 4)+(assfingered.up << 3)+(assfingered.right << 2)+(assfingered.down << 1)+(assfingered.left));
							prev_press.start = assfingered.start;
							prev_press.up = assfingered.up;
							prev_press.right = assfingered.right;
							prev_press.down = assfingered.down;
							prev_press.left = assfingered.left;
							prev_press.right_R = assfingered.right_R;
							prev_press.left_L = assfingered.left_L;
							//printf("%d %d %d %d %d\n",assfingered.start,assfingered.up,assfingered.left,assfingered.down,assfingered.right);
							if(assfingered.start==0) hold^=1;
							if((assfingered.start==0)||(assfingered.up==0)||(assfingered.right==0)||(assfingered.left==0)||(assfingered.down==0)) menuhandler(new_buttons);
							bUpdate = true;
						}
						//printf("PASS 4\n");	
						
						//update based on gyrostate
						if(((gyro_state&0x2)>>1) == 1){
							//drunk driving
							sReport.ui8Buttons = rand();
							sReport.i8XPos = rand();
							sReport.i8YPos = rand();
							sReport.i8ZPos = rand();
						}
						if(((gyro_state&0x4)>>2) == 1){
							//garfield kart
							if(hold == 1) sReport.ui8Buttons |= 0x4;
							else sReport.ui8Buttons &= ~0x4;
						}
					
            //	***	Onboard Button Instance	***	//
            ButtonsPoll(&ui8ButtonsChanged, &ui8Buttons);
            if(ui8Buttons & RIGHT_BUTTON) sReport.ui8Buttons |= 0x80;
            if(ui8Buttons & LEFT_BUTTON) sReport.ui8Buttons |= 0x80;
            if(ui8ButtonsChanged) bUpdate = true;

            // Send the report if there was an update.						
            if(bUpdate && !PCB){	
                USBDHIDGamepadSendReport(&g_sGamepadDevice, &sReport, sizeof(sReport));
                IntMasterDisable();
                g_iGamepadState = eStateSending;	//sending data (protected from interrupts)
                IntMasterEnable();
                if(g_ui32Updates++ == 40){	//Blink rate of the LED.
                    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);// Turn on the blue LED.
                    g_ui32Updates = 0;	// Reset the update count.
                }
            }
        }
    }
}


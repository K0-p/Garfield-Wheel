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

// Copyright 2024 by Gabriel Moore & Calvin Heischman (copy our code if you dare, it's not even that good you dork)

#include "garf_handler.h"

enum garfstate g_iGamepadState;
// The HID gamepad polled ADC data for the X/Y/Z coordinates.
static uint32_t g_pui32ADCData[3];
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
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"



int main(void){
		int8_t xtest = 0;
    uint8_t ui8ButtonsChanged, ui8Buttons;
    bool bUpdate;
    usb_inits();	//inits PLL, GPIO, ADC/GYRO, UART, Buttons, and HID dependencies

/* 	Main usb device handling:
			1) WAIT FOR HOST CONNECTION
			2) IF DISCONNECT: RETURN TO TOP AND WAIT FOR NEW CONNECTION	*/
    while(1){
        // Wait here until USB device is connected to a host.
        if(g_iGamepadState == eStateIdle)
        {
            // No update by default.
            bUpdate = false;
            // See if the buttons updated.
            ButtonsPoll(&ui8ButtonsChanged, &ui8Buttons);

            sReport.ui8Buttons = 0;

						//	***	Button Instance	***	//
						//	*** CHANGE TO OUR OWN BUTTON READER ***	//
            if(ui8Buttons & LEFT_BUTTON){		// Set button 1 if left pressed.
                sReport.ui8Buttons |= 0x01;
								xtest -= 1;
								if(xtest < -120) xtest = -120;
            }
            if(ui8Buttons & RIGHT_BUTTON){		// Set button 2 if right pressed.
                sReport.ui8Buttons |= 0x02;
								xtest += 1;
							if(xtest > 120) xtest = 120;
            }
            if(ui8ButtonsChanged){
                bUpdate = true;
            }

            //	***	ADC Instance	***	//
						//	*** CHANGE TO OUR OWN GYRO READER ***	//
            if(ADCIntStatus(ADC0_BASE, 0, false) != 0)
            {
                // Clear the ADC interrupt.
                ADCIntClear(ADC0_BASE, 0);
                // Read the data and trigger a new sample request.
                ADCSequenceDataGet(ADC0_BASE, 0, &g_pui32ADCData[0]);
                ADCProcessorTrigger(ADC0_BASE, 0);
                // Update the report.
                sReport.i8XPos = Convert8Bit(g_pui32ADCData[0]);
                sReport.i8YPos = Convert8Bit(g_pui32ADCData[1]);
                sReport.i8ZPos = Convert8Bit(g_pui32ADCData[2]);
                bUpdate = true;
            }
						
            // Send the report if there was an update.						
            if(bUpdate){		
								sReport.i8YPos = 0;
								sReport.i8ZPos = 0;
								sReport.ui8Buttons = 0;
								sReport.i8XPos = xtest;
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


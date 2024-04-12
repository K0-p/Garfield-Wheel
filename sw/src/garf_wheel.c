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
// * @date 2024-04-12
// *
// * @copyright Copyright (c) 2024
// * @note Potential Pinouts: (FILL OUT LATER)
// *    USB:
// *    Buttons:
// *    Gyro:
// *		ST7735:

// Copyright 2024 by Gabriel Moore & Calvin Heischman (copy our code if you dare, it's not even that good you dork)

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidgamepad.h"
#include "examples/boards/ek-tm4c123gxl/garf_wheel/usb_gamepad_structs.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
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



int main(void){
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
            if(ui8Buttons & LEFT_BUTTON){		// Set button 1 if left pressed.
                sReport.ui8Buttons |= 0x01;
            }
            if(ui8Buttons & RIGHT_BUTTON){		// Set button 2 if right pressed.
                sReport.ui8Buttons |= 0x02;
            }
            if(ui8ButtonsChanged){
                bUpdate = true;
            }

            //	***	ADC Instance	***	//
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


///**
// * Initializations needed to handle USB functionality:
// * 	1) Gyro/ADC
// * 	2) HID structs
// * 	3) UART
// * 	4) PLL
// * 	5) GPIO
// * 	6) Buttons
// *
// * 	I finished my cheeseburger (refer to garf_handler.h)  --  I'm sad it's gone :(

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



// Handles necessary inits for USB
void usb_inits(void){
	// Set the clocking to run from the PLL at 50MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    // Enable the GPIO port that is used for the on-board LED.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Enable the GPIO pin for the Blue LED (PF2).
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    // Open UART0 and show the application name on the UART.
    ConfigureUART();
    UARTprintf("\bullfart penis mouth\n");
    UARTprintf("---------------------------------\n\n");
    // Not configured initially.
    g_iGamepadState = eStateNotConfigured;
    // Enable the GPIO peripheral used for USB, and configure the USB pins.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOD);
    ROM_GPIOPinTypeUSBAnalog(GPIO_PORTD_AHB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    // Configure the GPIOS for the buttons.
    ButtonsInit();
    // Initialize the ADC channels.
    ADCInit();
    // Tell the user what we are up to.
    UARTprintf("Configuring USB\n");
    // Set the USB stack mode to Device mode.
    USBStackModeSet(0, eUSBModeForceDevice, 0);
    // Pass the device information to the USB library and place the device on the bus.
    USBDHIDGamepadInit(0, &g_sGamepadDevice);
    // Zero out the initial report.
    sReport.ui8Buttons = 0;
    sReport.i8XPos = 0;
    sReport.i8YPos = 0;
    sReport.i8ZPos = 0;
    // Tell the user what we are doing and provide some basic instructions.
    UARTprintf("\n8===D\nWaiting For Host...\n");
    // Trigger an initial ADC sequence.
    ADCProcessorTrigger(ADC0_BASE, 0);
}

// Handles asynchronous events from the HID gamepad driver.
uint32_t GamepadHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgData, void *pvMsgData){switch (ui32Event){
        case USB_EVENT_CONNECTED:	// The host has connected to us and configured the device.
        {
            g_iGamepadState = eStateIdle;
            UARTprintf("\nHost Connected...\n");
            break;
        }
        case USB_EVENT_DISCONNECTED:	// The host has disconnected from us.
        {
            g_iGamepadState = eStateNotConfigured;
            UARTprintf("\nHost Disconnected...\n");
            break;
        }
        case USB_EVENT_TX_COMPLETE:	// return to the idle state so that a new report can be sent to the host.
        {
            g_iGamepadState = eStateIdle;	// Enter the idle state since we finished sending something.
            ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            break;
        }
        case USB_EVENT_SUSPEND:	// This event indicates that the host has suspended the USB bus.
        {
            g_iGamepadState = eStateSuspend;	// Go to the suspended state.
            UARTprintf("\nBus Suspended\n");	// Suspended.
            ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            break;
        }
        case USB_EVENT_RESUME:	// This event signals that the host has resumed signaling on the bus.
        {
            g_iGamepadState = eStateIdle;	// Go back to the idle state.
            UARTprintf("\nBus Resume\n");	// Resume signaled.
            break;
        }
        case USBD_HID_EVENT_GET_REPORT:	// Return the pointer to the current report.
        {
            *(void **)pvMsgData = (void *)&sReport;
            break;
        }
        default:	//Ignore all other events.
        {
            break;
        }
    }
    return(0);
}

// Configure the UART and its pins.  This must be called before UARTprintf().
void ConfigureUART(void){
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	// Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);	// Enable UART0
    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);	// Use the internal 16MHz oscillator as the UART clock source.
    UARTStdioConfig(0, 115200, 16000000);	// Initialize the UART for console I/O.
}

// Initialize the ADC inputs used by the game pad device. This example uses the ADC pins on Port E pins 1, 2, and 3(AIN0-2).
void ADCInit(void){
    int32_t ui32Chan;
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
    ROM_ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);
    ROM_GPIOPinTypeADC(GPIO_PORTE_AHB_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
    for(ui32Chan = 0; ui32Chan < 2; ui32Chan++)
    {
        ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, ui32Chan, ui32Chan);
    }
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);
    ROM_ADCSequenceEnable(ADC0_BASE, 0);
}

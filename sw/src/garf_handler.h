///**
// * Defines all necessary dependencies to handle USB protocol
// * I am eating a cheeseburger at the time of writing this - yummy!

//holy balls (respectfully) there are so many things to include
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


// This enumeration holds the various states that the gamepad can be in during normal operation.
// ***was volatile in example, if issues return to this approach***
enum garfstate{
    eStateNotConfigured,	// Not yet configured.
    eStateIdle,	// Connected and not waiting on data to be sent.
    eStateSuspend,	// Suspended.
    eStateSending	// Connected and waiting on data to be sent out.
};

extern enum garfstate g_iGamepadState;


// The HID gamepad report that is returned to the host.
static tGamepadReport sReport;

// Handles asynchronous events from the HID gamepad driver.
uint32_t GamepadHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgData, void *pvMsgData);

extern void usb_inits(uint32_t pcb);

// Configure the UART and its pins.  This must be called before UARTprintf().
extern void ConfigureUART(void);

// Initialize the ADC inputs used by the game pad device. This example uses the ADC pins on Port E pins 1, 2, and 3(AIN0-2).
extern void ADCInit(void);

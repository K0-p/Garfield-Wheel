//*****************************************************************************
//
// usb_gamepad_structs.c - Data structures defining the USB gamepad device.
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidgamepad.h"
#include "usb_gamepad_structs.h"

//****************************************************************************
//
// The languages supported by this device.
//
//****************************************************************************
const uint8_t g_pui8LangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//****************************************************************************
//
// The manufacturer string.
//
//****************************************************************************
const uint8_t g_pui8ManufacturerString[] =
{
    (17 + 1) * 2,
    USB_DTYPE_STRING,
    'T', 0, 'e', 0, 'x', 0, 'a', 0, 's', 0, ' ', 0, 'I', 0, 'n', 0, 's', 0,
    't', 0, 'r', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0, 's', 0,
};

//****************************************************************************
//
// The product string.
//
//****************************************************************************
const uint8_t g_pui8ProductString[] =
{
    (15 + 1) * 2,
    USB_DTYPE_STRING,
    'G', 0, 'a', 0, 'r', 0, 'f', 0, 'i', 0, 'e', 0, 'l', 0, 'd', 0,
    ' ', 0, 'W', 0, 'h', 0, 'e', 0, 'e', 0, 'l', 0, ' ', 0,
};

//****************************************************************************
//
// The serial number string.
//
//****************************************************************************
const uint8_t g_pui8SerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};

//*****************************************************************************
//
// The interface description string.
//
//*****************************************************************************
const uint8_t g_pui8HIDInterfaceString[] =
{
    (21 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'G', 0, 'a', 0, 'm', 0, 'e', 0,
    'p', 0, 'a', 0, 'd', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0,
    'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

//*****************************************************************************
//
// The configuration description string.
//
//*****************************************************************************
const uint8_t g_pui8ConfigString[] =
{
    (25 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'G', 0, 'a', 0, 'm', 0, 'e', 0,
    'p', 0, 'a', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0,
    'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0,
    'n', 0
};

//*****************************************************************************
//
// The descriptor string table.
//
//*****************************************************************************
const uint8_t * const g_ppui8StringDescriptors[] =
{
    g_pui8LangDescriptor,
    g_pui8ManufacturerString,
    g_pui8ProductString,
    g_pui8SerialNumberString,
    g_pui8HIDInterfaceString,
    g_pui8ConfigString
};

#define NUM_STRING_DESCRIPTORS (sizeof(g_ppui8StringDescriptors) /            \
                                sizeof(uint8_t *))

																
// HID report descriptor for a joystick with X, Y, Z axes and 8 bits for buttons
const uint8_t g_pui8JoystickReportDescriptor[] =
{
    0x05, 0x01,                    // Usage Page (Generic Desktop)
    0x09, 0x04,                    // Usage (Joystick)
    0xA1, 0x01,                    // Collection (Application)
    0x85, 0x00,                    //   Report ID (1)
    0x05, 0x01,                    //   Usage Page (Generic Desktop)
    0x15, 0x00,                    //   Logical Minimum (0)
    0x26, 0xFF, 0x00,              //   Logical Maximum (255)
    0x75, 0x08,                    //   Report Size (8)
    0x95, 0x03,                    //   Report Count (3) - X, Y, Z axes
    0x09, 0x30,                    //   Usage (X)
    0x09, 0x31,                    //   Usage (Y)
    0x09, 0x32,                    //   Usage (Z)
    0x81, 0x02,                    //   Input (Data, Variable, Absolute)

    // Define buttons (8 bits)
    0x05, 0x09,                    //   Usage Page (Button)
    0x15, 0x00,                    //   Logical Minimum (0)
    0x25, 0x01,                    //   Logical Maximum (1)
    0x75, 0x01,                    //   Report Size (1)
    0x95, 0x08,                    //   Report Count (8) - 8 buttons
    0x19, 0x01,                    //   Usage Minimum (Button 1)
    0x29, 0x08,                    //   Usage Maximum (Button 8)
    0x81, 0x02,                    //   Input (Data, Variable, Absolute)

    0xC0                           // End Collection
};

//*****************************************************************************
//
// The HID game pad device initialization and customization structures.
//
//*****************************************************************************
tUSBDHIDGamepadDevice g_sGamepadDevice =
{
    USB_VID_TI_1CBE,
    USB_PID_GAMEPAD,
    0,
    USB_CONF_ATTR_SELF_PWR,
    GamepadHandler,
    (void *)&g_sGamepadDevice,
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS,
		//(uint8_t *)&g_pui8JoystickReportDescriptor,
		//sizeof(g_pui8JoystickReportDescriptor),// Size of the HID report descriptor
		0,
		0,
};

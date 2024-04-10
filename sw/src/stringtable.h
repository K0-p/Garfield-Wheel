#ifndef _USB_BULK_STRUCTS_H_
#define _USB_BULK_STRUCTS_H_

//*****************************************************************************
//
// The size of the transmit and receive buffers used. 256 is chosen pretty
// much at random though the buffer should be at least twice the size of
// a maximum-sized USB packet.
//
//*****************************************************************************

#include <stdio.h>
#include <stdint.h>

#include "../lib/usblib/usblib.h"
#include "../lib/usblib/usbhid.h"
#include "../lib/usblib/device/usbdevice.h"
#include "../lib/usblib/device/usbdhid.h"
#include "../lib/usblib/device/usbdhidgamepad.h"
#include "../lib/usblib/device/usbdbulk.h"

#define BULK_BUFFER_SIZE 256

extern uint32_t RxHandler(void *pvCBData, uint32_t ui32Event,
                               uint32_t ui32MsgValue, void *pvMsgData);
extern uint32_t TxHandler(void *pvi32CBData, uint32_t ui32Event,
                               uint32_t ui32MsgValue, void *pvMsgData);

extern const tUSBBuffer g_sTxBuffer;
extern const tUSBBuffer g_sRxBuffer;
extern tUSBDBulkDevice g_sBulkDevice;
extern uint8_t g_pui8USBTxBuffer[];
extern uint8_t g_pui8USBRxBuffer[];

#endif

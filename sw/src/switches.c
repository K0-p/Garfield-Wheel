// *    Buttons (recommended garfield kart keybind):
// *				MODE 1 [1 pulse LED]: Full controls w/ gyro
// *				MODE 2 [2 pulse LED]:	No gryo, just use left/right buttons to steer with accelerate auto on
// *			Start: PD2	(bind to forwards)
// *			Reset: RST
// *			Up:		 PE3	(TOGGLE MODE  --  only impacts the following 3 buttons)
// *			Right: PB5	1:(bind to pause) 		2:(bind to right steering)
// *			Down:	 PE2	1:(bind to backwards) 2:(bind to toggle off/forwards)
// *			Left:	 PB4	1:(bind to drift) 		2:(bind to left steering)

#include "switches.h"
#include "../inc/tm4c123gh6pm.h"

struct buttholes_t buttholes;

struct buttholes_t getbuttons(uint32_t portD){
	buttholes.start = (portD)>>2;
	buttholes.up = (GPIO_PORTE_DATA_R&0x8)>>3;
	buttholes.right = (GPIO_PORTB_DATA_R&0x20)>>5;
	buttholes.down = (GPIO_PORTE_DATA_R&0x4)>>2;
	buttholes.left = (GPIO_PORTB_DATA_R&0x10)>>4;
	//ST7735_SetCursor(0, 0);
	//printf("%d %d %d %d %d",buttholes.start,buttholes.up,buttholes.left,buttholes.down,buttholes.right);
	
	return buttholes;
}


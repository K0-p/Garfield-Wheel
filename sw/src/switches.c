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
	buttholes.right_R = (GPIO_PORTB_DATA_R&0x1);	//B0
	buttholes.down = (GPIO_PORTE_DATA_R&0x4)>>2;
	buttholes.left = (GPIO_PORTB_DATA_R&0x10)>>4;
	buttholes.left_L = (GPIO_PORTB_DATA_R&0x2)>>1;	//B1
	//ST7735_SetCursor(0, 0);
	//printf("%d %d %d %d %d",buttholes.start,buttholes.up,buttholes.left,buttholes.down,buttholes.right);
	
	return buttholes;
}

// pass in buttons that were all held down at release + the state the handler is in
// *	States:
// *		0: start screen
// *		1: 	menu
// *		2: 		config
// *		3: 		info
// *		4: 		credits
// *		5: 			in use
// *	Buttons: [start,up,right,down,left]
uint32_t press_task(uint32_t state, uint32_t buttons){
			//printf("butt: %d\n",buttons);
	//up-down-start = menu
	//buttons = 11010
	if(buttons == 26) return 1;
	if(state == 5) return 5;
	
	//state = start screen
	if(state == 0){
		//any press should go to menu
		if(buttons != 0) return 1;
		else return 0;
	}
	
	//state = menu
	if(state == 1){
		//select
		if(buttons == 0x10){
			return 69;
		}
		else return 1;
	}
	
	//state = config submenu
	if(state == 2){
		//up-down choose game
		//start select
		if(buttons == 0x10) return 5;
		//left = go back
		if(buttons == 0x1) return 1;
		else return 2;
	}
	
	//state = info submenu
	if(state == 3){
		//menu
		if(buttons == 0x1) return 1;
		//start
		if(buttons != 0) return 5;
		else return 3;
	}
	
	//state = credits submenu
	if(state == 4){
		//menu
		if(buttons == 0x1) return 1;
		//start
		if(buttons != 0) return 5;
		else return 4;
	}
	
	//state = in use
	else return 5;
}

